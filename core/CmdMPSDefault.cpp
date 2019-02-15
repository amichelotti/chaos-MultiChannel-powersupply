/*
CmdMPSDefault.cpp
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
#include "CmdMPSDefault.h"

#include <cmath>
#include  <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <sstream>
#define SCLAPP_ INFO_LOG(CmdMPSDefault) << "[" << getDeviceID() << "] "
#define SCLDBG_ DBG_LOG(CmdMPSDefault) << "[" << getDeviceID() << "] "
#define SCLERR_ ERR_LOG(CmdMPSDefault) << "[" << getDeviceID() << "] "
namespace own = driver::multichannelpowersupply;
namespace c_data =  chaos::common::data;
namespace chaos_batch = chaos::common::batch_command;
using namespace chaos::cu::control_manager;
BATCH_COMMAND_OPEN_DESCRIPTION(driver::multichannelpowersupply::,CmdMPSDefault,
	"Default command executed when no other commands in queue",
	"72a8c3fa-75e6-47da-9e48-2cbdc14fbe22")
BATCH_COMMAND_CLOSE_DESCRIPTION()


// return the implemented handler
uint8_t own::CmdMPSDefault::implementedHandler(){
	return      AbstractMultiChannelPowerSupplyCommand::implementedHandler()|chaos_batch::HandlerType::HT_Acquisition;
}
// empty set handler
void own::CmdMPSDefault::setHandler(c_data::CDataWrapper *data) {
	AbstractMultiChannelPowerSupplyCommand::setHandler(data);
	clearFeatures(chaos_batch::features::FeaturesFlagTypes::FF_SET_COMMAND_TIMEOUT);
	setBusyFlag(false);
	SCLAPP_ << "Set Handler Default "; 
	

	BC_NORMAL_RUNNING_PROPERTY
}
// empty acquire handler
void own::CmdMPSDefault::acquireHandler() {
	AbstractMultiChannelPowerSupplyCommand::outputRead();
	SCLDBG_ << "o_alarms: " << *o_alarms;
}
// empty correlation handler
void own::CmdMPSDefault::ccHandler() {
}
// empty timeout handler
bool own::CmdMPSDefault::timeoutHandler() {
	SCLDBG_ << "Timeout Handler Default "; 
	return false;
}
