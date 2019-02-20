/*
CmdMPSMainUnitPowerOn.cpp
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
#include "CmdMPSMainUnitPowerOn.h"

#include <cmath>
#include  <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <sstream>
#define SCLAPP_ INFO_LOG(CmdMPSMainUnitPowerOn) << "[" << getDeviceID() << "] "
#define SCLDBG_ DBG_LOG(CmdMPSMainUnitPowerOn) << "[" << getDeviceID() << "] "
#define SCLERR_ ERR_LOG(CmdMPSMainUnitPowerOn) << "[" << getDeviceID() << "] "
namespace own = driver::multichannelpowersupply;
namespace c_data =  chaos::common::data;
namespace chaos_batch = chaos::common::batch_command;
using namespace chaos::cu::control_manager;
BATCH_COMMAND_OPEN_DESCRIPTION_ALIAS(driver::multichannelpowersupply::,CmdMPSMainUnitPowerOn,CMD_MPS_MAINUNITPOWERON_ALIAS,
	"Switch on and off the Central Unit of the MultiChannelPowerSupply",
	"c1b2ce67-8c73-4254-9d7d-553bfc7c2f44")
BATCH_COMMAND_ADD_INT32_PARAM(CMD_MPS_MAINUNITPOWERON_ON_STATE,"0=OFF, 1=ON",chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_MANDATORY)
BATCH_COMMAND_CLOSE_DESCRIPTION()


// return the implemented handler
uint8_t own::CmdMPSMainUnitPowerOn::implementedHandler(){
	return      AbstractMultiChannelPowerSupplyCommand::implementedHandler()|chaos_batch::HandlerType::HT_Acquisition;
}
// empty set handler
void own::CmdMPSMainUnitPowerOn::setHandler(c_data::CDataWrapper *data) {
	AbstractMultiChannelPowerSupplyCommand::setHandler(data);
	SCLAPP_ << "Set Handler MainUnitPowerOn "; 
	setStateVariableSeverity(StateVariableTypeAlarmCU,"driver_command_error",chaos::common::alarm::MultiSeverityAlarmLevelClear);
	if(!data || !data->hasKey(CMD_MPS_MAINUNITPOWERON_ON_STATE))
	{
		SCLERR_ << "on_state not present in CDataWrapper";
		metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelError,"on_state not present in CDataWrapper" );
		setWorkState(false);
		BC_FAULT_RUNNING_PROPERTY
		return;
	}
	int32_t tmp_on_state=data->getInt32Value(CMD_MPS_MAINUNITPOWERON_ON_STATE);

	int err=0;
	if ((err=multichannelpowersupply_drv->MainUnitPowerOn(tmp_on_state)) != 0)
	{
		metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelError," command MainUnitPowerOn not acknowledged");
		setStateVariableSeverity(StateVariableTypeAlarmCU,"driver_command_error",chaos::common::alarm::MultiSeverityAlarmLevelHigh);
	}
	setWorkState(true);
	BC_NORMAL_RUNNING_PROPERTY
}
// empty acquire handler
void own::CmdMPSMainUnitPowerOn::acquireHandler() {
	SCLDBG_ << "Acquire Handler MainUnitPowerOn "; 
}
// empty correlation handler
void own::CmdMPSMainUnitPowerOn::ccHandler() {
	BC_END_RUNNING_PROPERTY;
}
// empty timeout handler
bool own::CmdMPSMainUnitPowerOn::timeoutHandler() {
	SCLDBG_ << "Timeout Handler MainUnitPowerOn "; 
	return false;
}
