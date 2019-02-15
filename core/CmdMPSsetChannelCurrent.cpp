/*
CmdMPSsetChannelCurrent.cpp
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
#include "CmdMPSsetChannelCurrent.h"

#include <cmath>
#include  <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <sstream>
#define SCLAPP_ INFO_LOG(CmdMPSsetChannelCurrent) << "[" << getDeviceID() << "] "
#define SCLDBG_ DBG_LOG(CmdMPSsetChannelCurrent) << "[" << getDeviceID() << "] "
#define SCLERR_ ERR_LOG(CmdMPSsetChannelCurrent) << "[" << getDeviceID() << "] "
namespace own = driver::multichannelpowersupply;
namespace c_data =  chaos::common::data;
namespace chaos_batch = chaos::common::batch_command;
using namespace chaos::cu::control_manager;
BATCH_COMMAND_OPEN_DESCRIPTION_ALIAS(driver::multichannelpowersupply::,CmdMPSsetChannelCurrent,CMD_MPS_SETCHANNELCURRENT_ALIAS,
	"set the current on a specified channel",
	"9cabc60b-2dde-4acc-886e-5058d2f8ceb3")
BATCH_COMMAND_ADD_INT32_PARAM(CMD_MPS_SETCHANNELCURRENT_SLOT,"the slot number of the channel",chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_MANDATORY)
BATCH_COMMAND_ADD_INT32_PARAM(CMD_MPS_SETCHANNELCURRENT_CHANNEL,"the channel to be set",chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_MANDATORY)
BATCH_COMMAND_ADD_DOUBLE_PARAM(CMD_MPS_SETCHANNELCURRENT_CURRENT,"the current(A) to be set",chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_MANDATORY)
BATCH_COMMAND_CLOSE_DESCRIPTION()


// return the implemented handler
uint8_t own::CmdMPSsetChannelCurrent::implementedHandler(){
	return      AbstractMultiChannelPowerSupplyCommand::implementedHandler()|chaos_batch::HandlerType::HT_Acquisition;
}
// empty set handler
void own::CmdMPSsetChannelCurrent::setHandler(c_data::CDataWrapper *data) {
	AbstractMultiChannelPowerSupplyCommand::setHandler(data);
	SCLAPP_ << "Set Handler setChannelCurrent "; 
	setStateVariableSeverity(StateVariableTypeAlarmCU,"driver_command_error",chaos::common::alarm::MultiSeverityAlarmLevelClear);
	if(!data || !data->hasKey(CMD_MPS_SETCHANNELCURRENT_SLOT))
	{
		SCLERR_ << "slot not present in CDataWrapper";
		metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelError,"slot not present in CDataWrapper" );
		setWorkState(false);
		BC_FAULT_RUNNING_PROPERTY
		return;
	}
	int32_t tmp_slot=data->getInt32Value(CMD_MPS_SETCHANNELCURRENT_SLOT);

	if(!data || !data->hasKey(CMD_MPS_SETCHANNELCURRENT_CHANNEL))
	{
		SCLERR_ << "channel not present in CDataWrapper";
		metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelError,"channel not present in CDataWrapper" );
		setWorkState(false);
		BC_FAULT_RUNNING_PROPERTY
		return;
	}
	int32_t tmp_channel=data->getInt32Value(CMD_MPS_SETCHANNELCURRENT_CHANNEL);

	if(!data || !data->hasKey(CMD_MPS_SETCHANNELCURRENT_CURRENT))
	{
		SCLERR_ << "current not present in CDataWrapper";
		metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelError,"current not present in CDataWrapper" );
		setWorkState(false);
		BC_FAULT_RUNNING_PROPERTY
		return;
	}
	double tmp_current=data->getDoubleValue(CMD_MPS_SETCHANNELCURRENT_CURRENT);

	int err=0;
	if ((err=multichannelpowersupply_drv->setChannelCurrent(tmp_slot,tmp_channel,tmp_current)) != 0)
	{
		metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelError," command setChannelCurrent not acknowledged");
		setStateVariableSeverity(StateVariableTypeAlarmCU,"driver_command_error",chaos::common::alarm::MultiSeverityAlarmLevelHigh);
	}
	setWorkState(true);
	BC_NORMAL_RUNNING_PROPERTY
}
// empty acquire handler
void own::CmdMPSsetChannelCurrent::acquireHandler() {
	SCLDBG_ << "Acquire Handler setChannelCurrent "; 
}
// empty correlation handler
void own::CmdMPSsetChannelCurrent::ccHandler() {
	BC_END_RUNNING_PROPERTY;
}
// empty timeout handler
bool own::CmdMPSsetChannelCurrent::timeoutHandler() {
	SCLDBG_ << "Timeout Handler setChannelCurrent "; 
	return false;
}
