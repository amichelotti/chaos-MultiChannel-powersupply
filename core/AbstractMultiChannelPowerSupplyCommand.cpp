/*
AbstractMultiChannelPowerSupplyCommand.cpp
!CHAOS
Created by CUGenerator

Copyright 2013 INFN, National Institute of Nuclear Physics
Licensed under the Apache License, Version 2.0 (the "License")
you may not use this file except in compliance with the License.
      You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
#include "AbstractMultiChannelPowerSupplyCommand.h"
#include <boost/format.hpp>
#define CMDCUINFO_ INFO_LOG(AbstractMultiChannelPowerSupplyCommand)
#define CMDCUDBG_ DBG_LOG(AbstractMultiChannelPowerSupplyCommand)
#define CMDCUERR_ ERR_LOG(AbstractMultiChannelPowerSupplyCommand)
using namespace driver::multichannelpowersupply;
namespace chaos_batch = chaos::common::batch_command;
using namespace chaos::cu::control_manager;
AbstractMultiChannelPowerSupplyCommand::AbstractMultiChannelPowerSupplyCommand() {
	multichannelpowersupply_drv = NULL;
}
AbstractMultiChannelPowerSupplyCommand::~AbstractMultiChannelPowerSupplyCommand() {
	if(multichannelpowersupply_drv)
		delete(multichannelpowersupply_drv);
	multichannelpowersupply_drv = NULL;
}
void AbstractMultiChannelPowerSupplyCommand::setHandler(c_data::CDataWrapper *data) {
	CMDCUDBG_ << "loading pointer for output channel"; 
	//get pointer to the output dataset variable
	o_status_id = getAttributeCache()->getRWPtr<int32_t>(DOMAIN_OUTPUT, "status_id");
	o_alarms = getAttributeCache()->getRWPtr<uint64_t>(DOMAIN_OUTPUT, "alarms"); 
	chVoltages=getAttributeCache()->getRWPtr<double>(DOMAIN_OUTPUT,"ChannelVoltages");
	chCurrents=getAttributeCache()->getRWPtr<double>(DOMAIN_OUTPUT,"ChannelCurrents");
	chStatus=getAttributeCache()->getRWPtr<int64_t>(DOMAIN_OUTPUT,"ChannelStatus");
	paramToShow=getAttributeCache()->getROPtr<char>(DOMAIN_OUTPUT, "otherChannelParamsToShow");
	auxiliaryAvailable=getAttributeCache()->getRWPtr<char>(DOMAIN_OUTPUT,"otherAvailableChannelParams");
	const int32_t *userTimeout=getAttributeCache()->getROPtr<int32_t>(DOMAIN_INPUT,"driver_timeout");
	//setting default timeout (usec)
	if (*userTimeout > 0)
		setFeatures(chaos_batch::features::FeaturesFlagTypes::FF_SET_COMMAND_TIMEOUT,(uint32_t) (*userTimeout)*1000);
	else
		setFeatures(chaos_batch::features::FeaturesFlagTypes::FF_SET_COMMAND_TIMEOUT,(uint32_t) 10000000);
	chaos::cu::driver_manager::driver::DriverAccessor *multichannelpowersupply_accessor = driverAccessorsErogator->getAccessoInstanceByIndex(0);
	if(multichannelpowersupply_accessor != NULL) {
		if(multichannelpowersupply_drv == NULL) {
			multichannelpowersupply_drv = new chaos::driver::multichannelpowersupply::ChaosMultiChannelPowerSupplyInterface(multichannelpowersupply_accessor);
		}
	}
	auxParamList.clear();
	size_t current, next=-1;
	std::string aCopy(paramToShow);
	do
	{
		current=next+1;
		next=aCopy.find_first_of(",",current);
		std::string par=aCopy.substr(current,next-current);
		std::string typ=getTypeOfAuxParam(par);
		auxParamTypeList.push_back(typ);
		auxParamList.push_back(par);
	} while (next != std::string::npos);
}
// return the implemented handler
uint8_t AbstractMultiChannelPowerSupplyCommand::implementedHandler() {
	return  chaos_batch::HandlerType::HT_Set | chaos_batch::HandlerType::HT_Correlation;
}
void AbstractMultiChannelPowerSupplyCommand::ccHandler() {

}
void AbstractMultiChannelPowerSupplyCommand::setWorkState(bool working_flag) {
	setBusyFlag(working_flag);
}
int32_t AbstractMultiChannelPowerSupplyCommand::outputRead() {
	std::string deviceString;
	int ret;
	//SCLDBG_ <<"Launching UpdateHV";
	if ((ret=multichannelpowersupply_drv->UpdateHV(deviceString)) != 0)
	{
		metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelError," cannot retrieve data  from PowerSupply");
		setStateVariableSeverity(StateVariableTypeAlarmCU,"driver_command_error",chaos::common::alarm::MultiSeverityAlarmLevelHigh);
		return ret;
	}
	else
	{
		setStateVariableSeverity(StateVariableTypeAlarmCU,"driver_command_error",chaos::common::alarm::MultiSeverityAlarmLevelClear);

		Json::Value json_parameter;
  		Json::Reader json_reader;
		//parse json string
		if (!json_reader.parse(deviceString, json_parameter))
		{
			CMDCUERR_ << "Bad Json parameter " << json_parameter <<" INPUT " <<deviceString;
			return -1;
		}
		else
		{
			int count=0;
			//const Json::Value &dataset_description = json_parameter["attributes"];
			for (Json::ValueIterator it = json_parameter.begin(); it != json_parameter.end(); it++)
			{
				const Json::Value &json_attribute_VMon = (*it)["VMon"];
				const Json::Value &json_attribute_IMon = (*it)["IMon"];
				const Json::Value &json_attribute_status = (*it)["status"];
				if (json_attribute_VMon.isNull() || (json_attribute_IMon.isNull()) || (json_attribute_status.isNull())		)
				{
					CMDCUERR_ << "Bad Json parameter " << json_parameter <<" INPUT " <<deviceString;
					metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelError," Unknown format data  from PowerSupply");
					setStateVariableSeverity(StateVariableTypeAlarmCU,"driver_command_error",chaos::common::alarm::MultiSeverityAlarmLevelHigh);
					BC_FAULT_RUNNING_PROPERTY
					return -1;
				}


				double VMon=json_attribute_VMon.asDouble();
				double IMon=json_attribute_IMon.asDouble();
				int64_t status=json_attribute_status.asInt64();
				chVoltages[count]=VMon;
				chCurrents[count]=IMon;
				chStatus[count]=status;
				for (int i=0;i < auxParamList.size();i++)
				{
					const Json::Value &tmpJson = (*it)[auxParamList[i]];
					std::string cmp=auxParamTypeList[i];
					double* tmpDPointer;
					int32_t* tmpIPointer;
					int64_t* tmpLPointer;
					//SCLDBG_ << "param:" << auxParamList[i] << " type "<<cmp << " tmpJson: "<<tmpJson.asString();
					
					if ((cmp=="double") || (cmp =="float"))
					{
					    tmpDPointer =getAttributeCache()->getRWPtr<double>(DOMAIN_OUTPUT,"Channel"+auxParamList[i]);
						tmpDPointer[count]=tmpJson.asDouble();
					}
					else if (cmp.find("int32")>=0)
					{
						tmpIPointer =getAttributeCache()->getRWPtr<int32_t>(DOMAIN_OUTPUT,"Channel"+auxParamList[i]);
						tmpIPointer[count]=tmpJson.asInt();
					}
					else if ((cmp.find("int64")>=0) || (cmp == "long"))
					{
						tmpLPointer =getAttributeCache()->getRWPtr<int64_t>(DOMAIN_OUTPUT,"Channel"+auxParamList[i]);
						tmpLPointer[count]=tmpJson.asInt64();
					}
				}
				//SCLDBG_ << "channel: " << count;
				//SCLDBG_ << "VMon:" << VMon << "  IMon: " <<IMon << "status: " << status;
				count++;


			}
		}
	}
	getAttributeCache()->setOutputDomainAsChanged();
	return 0;
}
std::string AbstractMultiChannelPowerSupplyCommand::getTypeOfAuxParam(std::string par)
{
	Json::Value json_parameter;
  	Json::Reader json_reader;
	  std::string retVal="";
	if (!json_reader.parse(this->auxiliaryAvailable, json_parameter))
	{
		metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelError," Unknown format data  from auxiliary parameters of PowerSupply");
	}
	else
	{
		if (json_parameter[par].isNull())
		{
			metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelError,par+ " not found  in auxiliary parameters of PowerSupply");
		}
		else
		{
			retVal=json_parameter[par].asString();
		}
	}
	return retVal;
}

int32_t AbstractMultiChannelPowerSupplyCommand::getProgressiveChannel(int32_t slot, int32_t channel)
{
	if ((slot==-1) || (channel==-1))
		return -1;

	const char* presentSlot=getAttributeCache()->getROPtr<char>(DOMAIN_OUTPUT, "presentSlotNumber");
	const char* chanXSlot= getAttributeCache()->getROPtr<char>(DOMAIN_OUTPUT, "channelsPerSlot");
	std::string presentSlotStr(presentSlot), chanXSlotStr(chanXSlot);
	std::vector<int32_t> slotVector;
	std::vector<int32_t> chanXSlotVector;
	size_t current, next=-1;
	do
	{
		current=next+1;
		next=presentSlotStr.find_first_of(" ",current);
		std::string valueStr=presentSlotStr.substr(current,next-current);
		slotVector.push_back(atoi(valueStr.c_str()));
	} while (next != std::string::npos);
	do
	{
		current=next+1;
		next=chanXSlotStr.find_first_of(" ",current);
		std::string valueStr=chanXSlotStr.substr(current,next-current);
		chanXSlotVector.push_back(atoi(valueStr.c_str()));
	} while (next != std::string::npos);
	int i;
	for (i=0; i < slotVector.size(); i++)
	{
		if (slot == slotVector[i])
			break;
	}
	if (i==slotVector.size())
		return -2;
	else
	{
		int sum=0;
		for (int j=0;j< i; j++)
			sum+= chanXSlotVector[j];
		
		if (channel >= chanXSlotVector[i])
			return -3;
		else
			return channel+sum;
	}


}