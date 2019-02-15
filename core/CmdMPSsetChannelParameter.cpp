/*
CmdMPSsetChannelParameter.cpp
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
#include "CmdMPSsetChannelParameter.h"

#include <cmath>
#include  <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <sstream>
#define SCLAPP_ INFO_LOG(CmdMPSsetChannelParameter) << "[" << getDeviceID() << "] "
#define SCLDBG_ DBG_LOG(CmdMPSsetChannelParameter) << "[" << getDeviceID() << "] "
#define SCLERR_ ERR_LOG(CmdMPSsetChannelParameter) << "[" << getDeviceID() << "] "
namespace own = driver::multichannelpowersupply;
namespace c_data =  chaos::common::data;
namespace chaos_batch = chaos::common::batch_command;
using namespace chaos::cu::control_manager;
BATCH_COMMAND_OPEN_DESCRIPTION_ALIAS(driver::multichannelpowersupply::,CmdMPSsetChannelParameter,CMD_MPS_SETCHANNELPARAMETER_ALIAS,
	"set a parameter for a specified channel",
	"28adebd6-3b00-42ad-b41f-525ee4e95cc1")
BATCH_COMMAND_ADD_INT32_PARAM(CMD_MPS_SETCHANNELPARAMETER_SLOT,"the slot number",chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_MANDATORY)
BATCH_COMMAND_ADD_INT32_PARAM(CMD_MPS_SETCHANNELPARAMETER_CHANNEL,"the channel to be set",chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_MANDATORY)
BATCH_COMMAND_ADD_STRING_PARAM(CMD_MPS_SETCHANNELPARAMETER_PARAMNAME,"the parameter name to be set",chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_MANDATORY)
BATCH_COMMAND_ADD_STRING_PARAM(CMD_MPS_SETCHANNELPARAMETER_PARAMVALUE,"the value of the parameter to be set",chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_MANDATORY)
BATCH_COMMAND_CLOSE_DESCRIPTION()


// return the implemented handler
uint8_t own::CmdMPSsetChannelParameter::implementedHandler(){
	return      AbstractMultiChannelPowerSupplyCommand::implementedHandler()|chaos_batch::HandlerType::HT_Acquisition;
}
// empty set handler
void own::CmdMPSsetChannelParameter::setHandler(c_data::CDataWrapper *data) {
	AbstractMultiChannelPowerSupplyCommand::setHandler(data);
	SCLAPP_ << "Set Handler setChannelParameter "; 
	setStateVariableSeverity(StateVariableTypeAlarmCU,"driver_command_error",chaos::common::alarm::MultiSeverityAlarmLevelClear);
	if(!data || !data->hasKey(CMD_MPS_SETCHANNELPARAMETER_SLOT))
	{
		SCLERR_ << "slot not present in CDataWrapper";
		metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelError,"slot not present in CDataWrapper" );
		setWorkState(false);
		BC_FAULT_RUNNING_PROPERTY
		return;
	}
	int32_t tmp_slot=data->getInt32Value(CMD_MPS_SETCHANNELPARAMETER_SLOT);

	if(!data || !data->hasKey(CMD_MPS_SETCHANNELPARAMETER_CHANNEL))
	{
		SCLERR_ << "channel not present in CDataWrapper";
		metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelError,"channel not present in CDataWrapper" );
		setWorkState(false);
		BC_FAULT_RUNNING_PROPERTY
		return;
	}
	int32_t tmp_channel=data->getInt32Value(CMD_MPS_SETCHANNELPARAMETER_CHANNEL);

	if(!data || !data->hasKey(CMD_MPS_SETCHANNELPARAMETER_PARAMNAME))
	{
		SCLERR_ << "paramName not present in CDataWrapper";
		metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelError,"paramName not present in CDataWrapper" );
		setWorkState(false);
		BC_FAULT_RUNNING_PROPERTY
		return;
	}
	std::string tmp_paramName=data->getStringValue(CMD_MPS_SETCHANNELPARAMETER_PARAMNAME);

	if(!data || !data->hasKey(CMD_MPS_SETCHANNELPARAMETER_PARAMVALUE))
	{
		SCLERR_ << "paramValue not present in CDataWrapper";
		metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelError,"paramValue not present in CDataWrapper" );
		setWorkState(false);
		BC_FAULT_RUNNING_PROPERTY
		return;
	}
	std::string tmp_paramValue=data->getStringValue(CMD_MPS_SETCHANNELPARAMETER_PARAMVALUE);

	int err=0;
	if ((err=multichannelpowersupply_drv->setChannelParameter(tmp_slot,tmp_channel,tmp_paramName,tmp_paramValue)) != 0)
	{
		metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelError," command setChannelParameter not acknowledged");
		setStateVariableSeverity(StateVariableTypeAlarmCU,"driver_command_error",chaos::common::alarm::MultiSeverityAlarmLevelHigh);
	}
	setWorkState(true);
	BC_NORMAL_RUNNING_PROPERTY
}
// empty acquire handler
void own::CmdMPSsetChannelParameter::acquireHandler() {
	SCLDBG_ << "Acquire Handler setChannelParameter "; 
}
// empty correlation handler
void own::CmdMPSsetChannelParameter::ccHandler() {
	BC_END_RUNNING_PROPERTY;
}
// empty timeout handler
bool own::CmdMPSsetChannelParameter::timeoutHandler() {
	SCLDBG_ << "Timeout Handler setChannelParameter "; 
	return false;
}
