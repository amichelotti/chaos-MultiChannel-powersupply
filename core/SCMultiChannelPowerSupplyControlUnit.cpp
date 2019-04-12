/*
SCMultiChannelPowerSupplyControlUnit.cpp
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
#include "SCMultiChannelPowerSupplyControlUnit.h"
#include <string>
#include <common/powersupply/core/AbstractPowerSupply.h>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <common/debug/core/debug.h>
#include <chaos/common/data/CDataWrapper.h>
#include <json/json.h>
#include "CmdMPSDefault.h"
#include "CmdMPSsetChannelVoltage.h"
#include "CmdMPSsetChannelCurrent.h"
#include "CmdMPSsetChannelParameter.h"
#include "CmdMPSPowerOn.h"
#include "CmdMPSMainUnitPowerOn.h"
using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::common::batch_command;
using namespace chaos::cu::control_manager::slow_command;
using namespace chaos::cu::driver_manager::driver;
using namespace chaos::cu::control_manager;
#define SCCUAPP INFO_LOG(SCMultiChannelPowerSupplyControlUnit)
#define SCCUDBG DBG_LOG(SCMultiChannelPowerSupplyControlUnit)
#define SCCUERR ERR_LOG(SCMultiChannelPowerSupplyControlUnit)


PUBLISHABLE_CONTROL_UNIT_IMPLEMENTATION(::driver::multichannelpowersupply::SCMultiChannelPowerSupplyControlUnit)

/*Construct a new CU with an identifier*/
::driver::multichannelpowersupply::SCMultiChannelPowerSupplyControlUnit::SCMultiChannelPowerSupplyControlUnit(const string &_control_unit_id,
			const string &_control_unit_param,const ControlUnitDriverList &_control_unit_drivers)
:  chaos::cu::control_manager::SCAbstractControlUnit(_control_unit_id,
			 _control_unit_param, _control_unit_drivers) {
	multichannelpowersupply_drv = NULL;
	SCCUAPP << "ALEDEBUG Constructing CU with load parameter " << _control_unit_param ;
	size_t current, next=-1;
	this->auxiliaryParamsToPush.clear();
	do
	{
		current=next+1;
		
		next=_control_unit_param.find_first_of(",",current);
		std::string par=_control_unit_param.substr(current,next-current);
		std::string trimmedPar=trim_copy(par);
		this->auxiliaryParamsToPush.push_back(trimmedPar);
	} while (next != string::npos);
}
/*Base Destructor*/
::driver::multichannelpowersupply::SCMultiChannelPowerSupplyControlUnit::~SCMultiChannelPowerSupplyControlUnit() {
	if (multichannelpowersupply_drv) {
		delete (multichannelpowersupply_drv);
	}
}
//handlers
//end handlers
void ::driver::multichannelpowersupply::SCMultiChannelPowerSupplyControlUnit::unitDefineActionAndDataset()  {
	installCommand(BATCH_COMMAND_GET_DESCRIPTION(CmdMPSDefault),true);
	installCommand(BATCH_COMMAND_GET_DESCRIPTION(CmdMPSsetChannelVoltage));
	installCommand(BATCH_COMMAND_GET_DESCRIPTION(CmdMPSsetChannelCurrent));
	installCommand(BATCH_COMMAND_GET_DESCRIPTION(CmdMPSsetChannelParameter));
	installCommand(BATCH_COMMAND_GET_DESCRIPTION(CmdMPSPowerOn));
	installCommand(BATCH_COMMAND_GET_DESCRIPTION(CmdMPSMainUnitPowerOn));
	chaos::cu::driver_manager::driver::DriverAccessor *multichannelpowersupply_accessor = getAccessoInstanceByIndex(0);
	if (multichannelpowersupply_accessor == NULL ) {
		throw chaos::CException(-1, "Cannot retrieve the requested driver", __FUNCTION__);
	}
	multichannelpowersupply_drv = new chaos::driver::multichannelpowersupply::ChaosMultiChannelPowerSupplyInterface(multichannelpowersupply_accessor);
	if (multichannelpowersupply_drv == NULL) {
		throw chaos::CException(-2, "Cannot allocate driver resources", __FUNCTION__);
	}
	
	SCCUAPP << "Reading slot config" ;
	if (multichannelpowersupply_drv->getSlotConfiguration(slotsID,channelsPerSlots) != 0)
	{
		throw chaos::CException(-3, "Cannot receive slots and channels info from driver", __FUNCTION__);
	}
	int32_t numOfChannels=0;

	this->channelsPerSlotStr.clear();
	this->idSlotPresent.clear();
	for (int i=0;i < channelsPerSlots.size();i++)
	{
		numOfChannels+= channelsPerSlots[i];
		this->channelsPerSlotStr+= to_string(channelsPerSlots[i])+ " ";
		this->idSlotPresent+=to_string(slotsID[i]) + " ";
	}
	auxiliaryParams="";
	this->raiseAuxiliaryParamError=false;
	multichannelpowersupply_drv->getChannelParametersDescription(auxiliaryParams);
	Json::Value json_parameter;
  	Json::Reader json_reader;
	

	addAttributeToDataSet("presentSlotNumber",
							"a string with the present slots",
							DataType::TYPE_STRING,
							DataType::Output, 256);
	addAttributeToDataSet("channelsPerSlot",
							"a string with the channels of each slot",
							DataType::TYPE_STRING,
							DataType::Output, 256);
	

	addAttributeToDataSet("Main_Status_Description",
							"a string with the description of the main unit status",
							DataType::TYPE_STRING,
							DataType::Output, 256);
	addAttributeToDataSet("status_id",
							"default status attribute",
							DataType::TYPE_INT32,
							DataType::Output);
	addAttributeToDataSet("Main_Alarms_Description",
							"a string with the description of the main unit alarms",
							DataType::TYPE_STRING,
							DataType::Output, 256);
	addAttributeToDataSet("alarms",
							"default alarms attribute",
							DataType::TYPE_INT64,
							DataType::Output);
	addBinaryAttributeAsSubtypeToDataSet("ChannelVoltages",
							"the voltage readout",
							chaos::DataType::SUB_TYPE_DOUBLE,
							numOfChannels*sizeof(double),
							chaos::DataType::Output);
	addBinaryAttributeAsSubtypeToDataSet("ChannelCurrents",
							"the current readout",
							chaos::DataType::SUB_TYPE_DOUBLE,
							numOfChannels*sizeof(double),
							chaos::DataType::Output);
	addBinaryAttributeAsSubtypeToDataSet("ChannelStatus",
							"the status mask for each channel",
							chaos::DataType::SUB_TYPE_INT64,
							numOfChannels*sizeof(int64_t),
							chaos::DataType::Output);

	addBinaryAttributeAsSubtypeToDataSet("ChannelAlarms",
							"the alarm mask for each channel",
							chaos::DataType::SUB_TYPE_INT64,
							numOfChannels*sizeof(int64_t),
							chaos::DataType::Output);

	addAttributeToDataSet("otherAvailableChannelParams",
							"a string with the complemementary parameters that can be set on the driver",
							DataType::TYPE_STRING,
							DataType::Output, 256);
	
	addAttributeToDataSet("otherChannelParamsToShow",
							"a string with the name of the complementary parameters that user want to be pushed on output",
							DataType::TYPE_STRING,
							DataType::Output, 256);


	addAttributeToDataSet("driver_timeout",
							"Driver timeout in milliseconds",
							DataType::TYPE_INT32,
							DataType::Input);
	addAttributeToDataSet("GeneratorBehaviour",
							"1 if it drives current. 2 if it drives Voltages 0 to suppress any check over voltage and current",
							DataType::TYPE_INT32,
							DataType::Input);
	addAttributeToDataSet("SetResolution",
							"Resolution of the setting parameter",
							DataType::TYPE_DOUBLE,
							DataType::Input);
	
	addAttributeToDataSet("MaxChannelSetValue",
							"The maximum of the current OR voltage that could be accepted for a channel",
							DataType::TYPE_DOUBLE,
							DataType::Input);
	addAttributeToDataSet("MinChannelSetValue",
							"The minimum of the current OR voltage that could be accepted for a channel",
							DataType::TYPE_DOUBLE,
							DataType::Input);

		addBinaryAttributeAsSubtypeToDataSet("ChannelSetValue",
							"the set value imposed for current OR voltage",
							chaos::DataType::SUB_TYPE_DOUBLE,
							numOfChannels*sizeof(double),
							chaos::DataType::Input);
	if (!json_reader.parse(auxiliaryParams, json_parameter))
	{
		SCCUERR << "Bad Json parameter " << json_parameter <<" INPUT " << auxiliaryParams;
		auxiliaryParams="";
		this->raiseAuxiliaryParamError=true;
	}
	else
	{
		for (int i=0; i < this->auxiliaryParamsToPush.size();i++)
		{
			bool found=false;
			std::string kindOf;
			std::string parName;
			SCCUDBG << "ALEDEBUG search : " << this->auxiliaryParamsToPush[i];
			std::vector<std::string> listParam= json_parameter.getMemberNames();
			for (int j=0; j < listParam.size(); j++)
			{
				SCCUDBG << "ALEDEBUG " << listParam[j];
				if (listParam[j] == this->auxiliaryParamsToPush[i])
				{
					found=true;
					parName=listParam[j];
					kindOf=json_parameter[listParam[j]].asString();
					break;
				}
			}
			if (found)
			{
				bool conversionError=false;
				std::string toRecord="Channel"+parName;
				chaos::DataType::BinarySubtype theSubType;
				size_t dimension;
				if (kindOf == "double")
				{
					theSubType=chaos::DataType::SUB_TYPE_DOUBLE;
					dimension=sizeof(double);
				}
				else if (kindOf == "float")
				{
					theSubType=chaos::DataType::SUB_TYPE_DOUBLE;
					dimension=sizeof(double);
				}
				else if (kindOf == "int")
				{
					theSubType=chaos::DataType::SUB_TYPE_INT32;
					dimension=sizeof(int32_t);
				}
				else if (kindOf.find("int64")>=0)
				{
					theSubType=chaos::DataType::SUB_TYPE_INT64;
					dimension=sizeof(int64_t);
				}
				else if (kindOf.find("int32")>=0)
				{
					theSubType=chaos::DataType::SUB_TYPE_INT32;
					dimension=sizeof(int32_t);
				}
				else
				{
					conversionError=true;
					this->raiseAuxiliaryParamError=true;
					SCCUERR << "ALEDEBUG conversion error";
				}
				if (!conversionError)
				{
					addBinaryAttributeAsSubtypeToDataSet(toRecord,
							"the "+parName+" readout",
							theSubType,
							numOfChannels*dimension,
							chaos::DataType::Output);
				}
			}
			else
			{
				SCCUERR << "ALEDEBUG NOT FOUND";
				this->raiseAuxiliaryParamError=true;
			}

		}



	}





	addStateVariable(StateVariableTypeAlarmCU,"driver_command_error",
		"last driver user command has failed");
	addStateVariable(StateVariableTypeAlarmCU,"driver_reading_error",
		"error occurred while reading data from driver");
	
	addStateVariable(StateVariableTypeAlarmCU,"auxiliary_parameter_configuration_error",
		"raised when in configuration we ask to monitor an auxiliary parameter not existent or not convertible, or with wrong sintax");
	addStateVariable(StateVariableTypeAlarmCU,"channel_out_of_set",
		"notified when a channel shifted from setpoint");
	addStateVariable(StateVariableTypeAlarmCU,"bad_command_parameter",
		"notified when a command is issued with wrong parameters");
	addStateVariable(StateVariableTypeAlarmCU,"setPoint_not_reached",
		"notified when a channel voltage or current setPoint is not reached after a set  command");



}
void ::driver::multichannelpowersupply::SCMultiChannelPowerSupplyControlUnit::unitDefineCustomAttribute() {
}
// Abstract method for the initialization of the control unit
void ::driver::multichannelpowersupply::SCMultiChannelPowerSupplyControlUnit::unitInit() {
		char* chanNum=NULL, *slots=NULL, *auxPar=NULL, *showPar=NULL;
		chanNum = getAttributeCache()->getRWPtr<char>(DOMAIN_OUTPUT, "channelsPerSlot"); 
		chanNum=strncpy(chanNum,this->channelsPerSlotStr.c_str(),256);
		slots= getAttributeCache()->getRWPtr<char>(DOMAIN_OUTPUT, "presentSlotNumber"); 
		slots=strncpy(slots,this->idSlotPresent.c_str(),256);
		auxPar= getAttributeCache()->getRWPtr<char>(DOMAIN_OUTPUT, "otherAvailableChannelParams");
		auxPar=strncpy(auxPar,this->auxiliaryParams.c_str(),256);
		if (this->raiseAuxiliaryParamError)
		{
			setStateVariableSeverity(StateVariableTypeAlarmCU,"auxiliary_parameter_configuration_error",chaos::common::alarm::MultiSeverityAlarmLevelWarning);
		}
		else
		{
			std::string tmpStr;
			for (int i=0;i < this->auxiliaryParamsToPush.size(); i++)
			{
				if (i>0)
					tmpStr+=",";
				tmpStr+=this->auxiliaryParamsToPush[i];
			}
			showPar=getAttributeCache()->getRWPtr<char>(DOMAIN_OUTPUT, "otherChannelParamsToShow");
			showPar=strncpy(showPar,tmpStr.c_str(),256);

		}

}
// Abstract method for the start of the control unit
void ::driver::multichannelpowersupply::SCMultiChannelPowerSupplyControlUnit::unitStart() {
		std::string deviceString;
		const int32_t* kindOfGenerator=getAttributeCache()->getROPtr<int32_t>(DOMAIN_INPUT,"GeneratorBehaviour");
		
		int ret;
		if (*kindOfGenerator != ::common::multichannelpowersupply::MPS_NOT_SPECIFIED)
		{
			double* chVal=(*kindOfGenerator == ::common::multichannelpowersupply::MPS_VOLTAGE_GENERATOR) ? getAttributeCache()->getRWPtr<double>(DOMAIN_OUTPUT,"ChannelVoltages") :  getAttributeCache()->getRWPtr<double>(DOMAIN_OUTPUT,"ChannelCurrents") ;
			double* inputVal=getAttributeCache()->getRWPtr<double>(DOMAIN_INPUT,"ChannelSetValue");
			if ((ret=multichannelpowersupply_drv->UpdateHV(deviceString)) == 0) 
			{
				Json::Value json_parameter;
				Json::Reader json_reader;
				//parse json string
				if (!json_reader.parse(deviceString, json_parameter))
				{
					SCCUERR << "Bad Json parameter " << json_parameter <<" INPUT " <<deviceString;
				}
				else
				{
					int count=0;
					for (Json::ValueIterator it = json_parameter.begin(); it != json_parameter.end(); it++)
					{
						const Json::Value &json_attribute_VMon = (*it)["VMon"];
						const Json::Value &json_attribute_IMon = (*it)["IMon"];
						if (json_attribute_VMon.isNull() || (json_attribute_IMon.isNull()) )
						{
							SCCUERR << "Bad Json parameter " << json_parameter <<" INPUT " <<deviceString;

						}
						else
						{
							double readVal=(*kindOfGenerator == ::common::multichannelpowersupply::MPS_VOLTAGE_GENERATOR)? json_attribute_VMon.asDouble() : json_attribute_IMon.asDouble();
							chVal[count]=readVal;
							inputVal[count]=readVal;
						}
					++count;
					}
				}
				getAttributeCache()->setOutputDomainAsChanged();
				getAttributeCache()->setInputDomainAsChanged();
			}//driver error received
		}//unspecified kind of generator

}
// Abstract method for the stop of the control unit
void ::driver::multichannelpowersupply::SCMultiChannelPowerSupplyControlUnit::unitStop()  {
}
// Abstract method for deinit the control unit
void ::driver::multichannelpowersupply::SCMultiChannelPowerSupplyControlUnit::unitDeinit() {
	SCCUAPP << "deinitializing ";
}
	//! restore the control unit to snapshot
#define RESTORE_LAPP SCCUAPP << "[RESTORE-" <<getCUID() << "] "
#define RESTORE_LERR SCCUERR << "[RESTORE-" <<getCUID() << "] "
#define RESTORE_LDBG SCCUDBG << "[RESTORE-" << getCUID() << "] "
bool ::driver::multichannelpowersupply::SCMultiChannelPowerSupplyControlUnit::unitRestoreToSnapshot(chaos::cu::control_manager::AbstractSharedDomainCache *const snapshot_cache)  {
	uint64_t start_restore_time= chaos::common::utility::TimingUtil::getTimeStamp();
	try
	{
		int32_t* genKind = getAttributeCache()->getRWPtr<int32_t>(DOMAIN_INPUT, "GeneratorBehaviour");
		if (snapshot_cache == NULL)
		{
			RESTORE_LERR << "cache nulla";
			return false;
		}
		if (snapshot_cache->getSharedDomain(DOMAIN_INPUT).hasAttribute("SetResolution"))
		{
			double restore_channel_resolution = *snapshot_cache->getAttributeValue(DOMAIN_INPUT, "SetResolution")->getValuePtr<double>();
			double* chanRes = getAttributeCache()->getRWPtr<double>(DOMAIN_INPUT, "SetResolution");
			if (chanRes != NULL)
			{
				RESTORE_LDBG << "Restoring channel resolution";
			    *chanRes=restore_channel_resolution;
				getAttributeCache()->setInputDomainAsChanged();
			}
			else
			{
				RESTORE_LDBG << "NOT Restoring channel resolution because of null";
			}
		}
		RESTORE_LDBG << "Restore Check if  cache for status_id";
		if (!snapshot_cache->getSharedDomain(DOMAIN_OUTPUT).hasAttribute("status_id"))
		{
			RESTORE_LERR << " missing status_id to restore";
			return false;
		}
		int32_t restore_power_sp = *snapshot_cache->getAttributeValue(DOMAIN_OUTPUT, "status_id")->getValuePtr<int32_t>();
		RESTORE_LDBG << "Restore in cache status is " << restore_power_sp;
		restore_power_sp=(int32_t)((restore_power_sp & ::common::powersupply::POWER_SUPPLY_STATE_ON) != 0);
    	RESTORE_LDBG << "Restore Trying to set power at " << restore_power_sp;
		multichannelpowersupply_drv->MainUnitPowerOn(restore_power_sp);

		char* snapParameterToShow= snapshot_cache->getAttributeValue(DOMAIN_OUTPUT,"otherChannelParamsToShow")->getValuePtr<char>();
		//RESTORE_LDBG << "Auxiliary restore param "<< snapParameterToShow;
		std::string parToShowConverted=snapParameterToShow;
		size_t current,next=-1;
		do
		{
			current=next+1;
			next=parToShowConverted.find_first_of(",",current);
			std::string par=parToShowConverted.substr(current,next-current);
			std::string trimmedPar=trim_copy(par);
			std::string parType="";
			size_t lenPar=getLengthOfAuxParameter(trimmedPar, parType);
			if (lenPar == 0)
			{
				RESTORE_LERR << "Cannot restore "<<trimmedPar <<" unknown size";
				return false;
			}
			std::string chanPar="Channel"+trimmedPar;
			RESTORE_LDBG << "Auxiliary restore param "<< trimmedPar;
			if (!snapshot_cache->getSharedDomain(DOMAIN_OUTPUT).hasAttribute(chanPar))
			{
				RESTORE_LERR << "Cannot restore "<< chanPar <<" : not present in snapshot";
				return false;
			}
			
			CDataVariant chanStat = snapshot_cache->getAttributeValue(DOMAIN_OUTPUT, chanPar)->getAsVariant();
			std::string readFromSnap=chanStat.asString();

			RESTORE_LDBG << "ALEDEBUG: length "<<readFromSnap.length() ;
			size_t arrLen=readFromSnap.length()/sizeof(int64_t);
			const char* data=readFromSnap.c_str();

			int slot, chan,ret=0;
			for (int i=0;i < arrLen; i++)
			{
				RESTORE_LDBG << "before getAsString "<< i <<" ";
				std::string parVal=getParAsString(data,i,parType);	
				if (this->getSlotAndChannel(i,slot,chan))
				{
					RESTORE_LDBG << "RESTORING: "<< trimmedPar <<" slot"<< slot << " chan "<<chan << "  value is: " <<parVal;
					ret+=multichannelpowersupply_drv->setChannelParameter(slot,chan,trimmedPar,parVal);

					std::auto_ptr<CDataWrapper> cmd_pack(new CDataWrapper());
  					cmd_pack->addInt32Value(CMD_MPS_SETCHANNELPARAMETER_SLOT, slot);
					cmd_pack->addInt32Value(CMD_MPS_SETCHANNELPARAMETER_CHANNEL, chan);
					cmd_pack->addStringValue(CMD_MPS_SETCHANNELPARAMETER_PARAMNAME,trimmedPar);
					cmd_pack->addStringValue(CMD_MPS_SETCHANNELPARAMETER_PARAMVALUE,parVal);

  //send command
					submitBatchCommand(CMD_MPS_SETCHANNELPARAMETER_ALIAS,
										cmd_pack.release(),
										cmd_id,
										0,
										50,
										SubmissionRuleType::SUBMIT_NORMAL);



					sleep(1); //lento non dovrebbe crepare
					RESTORE_LDBG << "after sleep "<< slot <<" " << chan;
				}
				
			}



		
		} while (next != string::npos);


		/*RESTORE_LDBG << "Restore Check if  cache for channelVoltages";
		if (snapshot_cache->getSharedDomain(DOMAIN_OUTPUT).hasAttribute("ChannelVoltages"))
		{
			CDataVariant chanVoltages = snapshot_cache->getAttributeValue(DOMAIN_OUTPUT, "ChannelVoltages")->getAsVariant();
			std::string readFromSnap=chanVoltages.asString();
			RESTORE_LDBG << "ALEDEBUG:got ChannelVoltages length "<<readFromSnap.length() ;
			size_t arrLen=readFromSnap.length()/sizeof(double);
			char* data=(char*)readFromSnap.c_str();
			int slot, chan,ret=0;
			for (int i=0;i < arrLen; i++)
			{
				double setValue = ((double*)data)[i];
				//stringstream ss;
				//ss << setValue;
				if (this->getSlotAndChannel(i,slot,chan))
				{
					RESTORE_LDBG << "RESTORING: "<< "Voltages" <<" slot"<< slot << " chan "<<chan << "  value is: " << setValue;
					std::string outStr;
					//ret+=multichannelpowersupply_drv->getChannelParametersDescription(outStr);
					//ret+=multichannelpowersupply_drv->setChannelVoltage(slot,chan,setValue);
					usleep(300000); //lento non dovrebbe crepare
				
				}
				
			}
		}
		//check if in the restore cache we have all information we need
		RESTORE_LDBG << "Restore Check if  cache for channelStatus";
		if (snapshot_cache->getSharedDomain(DOMAIN_OUTPUT).hasAttribute("ChannelStatus"))
		{
			CDataVariant chanStat = snapshot_cache->getAttributeValue(DOMAIN_OUTPUT, "ChannelStatus")->getAsVariant();
			std::string readFromSnap=chanStat.asString();
			RESTORE_LDBG << "ALEDEBUG:got ChannelStatus length "<<readFromSnap.length() ;
			size_t arrLen=readFromSnap.length()/sizeof(int64_t);
			char* data=(char*)readFromSnap.c_str();
			int slot, chan,ret=0;
			for (int i=0;i < arrLen; i++)
			{
				uint64_t theState= ((uint64_t*)data)[i];
				if (this->getSlotAndChannel(i,slot,chan))
				{
					int val=(theState==2)? 1:0;
					RESTORE_LDBG << "RESTORING: "<< "Status" <<" slot"<< slot << " chan "<<chan << "  value is: " << theState;
					ret+=multichannelpowersupply_drv->PowerOn(slot,chan,val);
					sleep(1);
				}
				

			}
		}*/
	}
	catch (CException &ex)
	{
		uint64_t restore_duration_in_ms = chaos::common::utility::TimingUtil::getTimeStamp() - start_restore_time;
		RESTORE_LERR << "[metric] Restore has fault in " << restore_duration_in_ms << " milliseconds";
		throw ex;
	}
	return false;
}
uint32_t  ::driver::multichannelpowersupply::SCMultiChannelPowerSupplyControlUnit::getTotalChannels()
{
	const char* chanXSlot= getAttributeCache()->getROPtr<char>(DOMAIN_OUTPUT, "channelsPerSlot");
	std::vector<int32_t> chanXSlotVector;
	size_t current, next=-1;
	std::string chanXSlotStr(chanXSlot);
	do
	{
		current=next+1;
		next=chanXSlotStr.find_first_of(" ",current);
		std::string valueStr=chanXSlotStr.substr(current,next-current);
		chanXSlotVector.push_back(atoi(valueStr.c_str()));
	} while (next != std::string::npos);
	int sum=0;
	for (int i=0; i < chanXSlotVector.size(); i++)
	{
		sum+=chanXSlotVector[i];
	}
	return sum;
}

size_t ::driver::multichannelpowersupply::SCMultiChannelPowerSupplyControlUnit::getLengthOfAuxParameter(std::string param,std::string& tipo)
{
	size_t ret=0;
	Json::Value json_parameter;
  	Json::Reader json_reader;
	if (!json_reader.parse(auxiliaryParams, json_parameter))
	{
		SCCUERR << "Bad Json parameter " << json_parameter <<" INPUT " << auxiliaryParams;
		auxiliaryParams="";
		this->raiseAuxiliaryParamError=true;
	}
	else
	{
		std::vector<std::string> listParam= json_parameter.getMemberNames();
		for (int j=0; j < listParam.size(); j++)
		{
			SCCUDBG << "ALEDEBUG " << listParam[j];
			if (listParam[j] == param)
			{
				std::string kindOf=json_parameter[listParam[j]].asString();
				
				if (kindOf == "double")
				{
					tipo="double";
					return sizeof(double);
				}
				else if (kindOf == "float")
				{
					tipo="double";
					return sizeof(double);
				}
				else if (kindOf == "int")
				{
					tipo="int32_t";
					return sizeof(int32_t);
				}
				else if (kindOf.find("int64")>=0)
				{
					tipo="int64_t";
					return sizeof(int64_t);
				}
				else if (kindOf.find("int32")>=0)
				{
					tipo="int32_t";
					return  sizeof(int32_t);
				}
				else
				{
					tipo="";
					return 0;
				}
				break;
			}
		}
			
	}
	return 0;
}

namespace driver
{
	std::string      multichannelpowersupply::SCMultiChannelPowerSupplyControlUnit::getParAsString(const char* rawData,int arrayCount,std::string tipo)
	{
		std::string toRet;
		stringstream vv ;
		vv.str("");
		if (tipo=="double")
		{
			double tmp=((double*)rawData)[arrayCount];
			vv << tmp;
		}
		else if (tipo == "int32_t")
		{
			int32_t tmp=((int32_t*)rawData)[arrayCount];
			vv << tmp;
		}
		else if (tipo == "int64_t")
		{
			int64_t tmp=((int64_t*)rawData)[arrayCount];
			vv << tmp;
		}

		return vv.str();
	}
	bool  multichannelpowersupply::SCMultiChannelPowerSupplyControlUnit::getSlotAndChannel(int progressive, int& slot, int&chan)
	{
		int prgCount =0;
		for (int i= 0; i < this->slotsID.size();i++)
		{
			int32_t chanThisSlot=this->channelsPerSlots[i];
			prgCount=i*chanThisSlot;
			if ((progressive - prgCount) < chanThisSlot)
			{
				slot= this->slotsID[i];
				chan = progressive - prgCount;
				return true;
			}

		}

		return false;
	}
}


















bool ::driver::multichannelpowersupply::SCMultiChannelPowerSupplyControlUnit::waitOnCommandID(uint64_t cmd_id) 
{
	 ChaosUniquePtr<chaos::common::batch_command::CommandState> cmd_state;
do { 
cmd_state = getStateForCommandID(cmd_id);
if (!cmd_state.get()) break;
switch (cmd_state->last_event) 
{
case BatchCommandEventType::EVT_QUEUED:
SCCUAPP << cmd_id << " -> QUEUED";
break;
case BatchCommandEventType::EVT_RUNNING:
SCCUAPP << cmd_id << " -> RUNNING"; 
break;
case BatchCommandEventType::EVT_WAITING:
SCCUAPP << cmd_id << " -> WAITING";
break;
case BatchCommandEventType::EVT_PAUSED:
SCCUAPP << cmd_id << " -> PAUSED";
break;
case BatchCommandEventType::EVT_KILLED:
SCCUAPP << cmd_id << " -> KILLED";
break;
case BatchCommandEventType::EVT_COMPLETED:
SCCUAPP << cmd_id << " -> COMPLETED";
break;
case BatchCommandEventType::EVT_FAULT:
    SCCUAPP << cmd_id << " -> FAULT";
break;
}
usleep(500000);
} while (cmd_state->last_event != BatchCommandEventType::EVT_COMPLETED &&
        cmd_state->last_event != BatchCommandEventType::EVT_FAULT &&
    cmd_state->last_event != BatchCommandEventType::EVT_KILLED);
return (cmd_state.get() &&
cmd_state->last_event == BatchCommandEventType::EVT_COMPLETED);

}
inline std::string trim_right_copy(const std::string& s, 
								  const std::string& delimiters=" \f\n\r\t\v")
								  {
									  return s.substr(0,s.find_last_not_of(delimiters)+1);
								  }
inline std::string trim_left_copy(const std::string &s,
 								  const std::string& delimiters=" \f\n\r\t\v")
								   {
									   return s.substr(s.find_first_not_of(delimiters));
								   }
inline std::string trim_copy(const std::string &s, const std::string& delimiters=" \f\n\r\t\v")
{
	return trim_left_copy(trim_right_copy(s,delimiters),delimiters);
}

