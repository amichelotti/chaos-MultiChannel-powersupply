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
	chaos::cu::driver_manager::driver::DriverAccessor *multichannelpowersupply_accessor = getAccessoInstanceByIndex(0);
	if (multichannelpowersupply_accessor == NULL ) {
		throw chaos::CException(-1, "Cannot retrieve the requested driver", __FUNCTION__);
	}
	multichannelpowersupply_drv = new chaos::driver::multichannelpowersupply::ChaosMultiChannelPowerSupplyInterface(multichannelpowersupply_accessor);
	if (multichannelpowersupply_drv == NULL) {
		throw chaos::CException(-2, "Cannot allocate driver resources", __FUNCTION__);
	}
	std::vector<int>  slotsID;
	std::vector<int>  channelsPerSlots;
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

	addAttributeToDataSet("presentSlotNumber",
							"a string with the present slots",
							DataType::TYPE_STRING,
							DataType::Output, 256);
	addAttributeToDataSet("channelsPerSlot",
							"a string with the channels of each slot",
							DataType::TYPE_STRING,
							DataType::Output, 256);
	addAttributeToDataSet("otherAvailableChannelParams",
							"a string with the complemementary parameters that can be set on the driver",
							DataType::TYPE_STRING,
							DataType::Output, 256);
	
	addAttributeToDataSet("otherChannelParamsToShow",
							"a string with the name of the complementary parameters that user want to be pushed on output",
							DataType::TYPE_STRING,
							DataType::Output, 256);

	addAttributeToDataSet("status_id",
							"default status attribute",
							DataType::TYPE_INT32,
							DataType::Output);
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

	addAttributeToDataSet("driver_timeout",
							"Driver timeout in milliseconds",
							DataType::TYPE_INT32,
							DataType::Input);
	addAttributeToDataSet("GeneratorBehaviour",
							"1 if it drives current. 2 if it drives Voltages 0 to suppress any check over voltage and current",
							DataType::TYPE_INT32,
							DataType::Input);
	addStateVariable(StateVariableTypeAlarmCU,"driver_command_error",
		"default driver communication error");
	
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
bool ::driver::multichannelpowersupply::SCMultiChannelPowerSupplyControlUnit::unitRestoreToSnapshot(chaos::cu::control_manager::AbstractSharedDomainCache *const snapshot_cache)  {
	return false;
}
bool ::driver::multichannelpowersupply::SCMultiChannelPowerSupplyControlUnit::waitOnCommandID(uint64_t cmd_id) {
	 ChaosUniquePtr<chaos::common::batch_command::CommandState> cmd_state;
do { 
cmd_state = getStateForCommandID(cmd_id);
if (!cmd_state.get()) break;
switch (cmd_state->last_event) {
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