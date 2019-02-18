/*
CmdMPSsetChannelVoltage.cpp
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
#include "CmdMPSsetChannelVoltage.h"

#include <cmath>
#include  <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <sstream>
#define SCLAPP_ INFO_LOG(CmdMPSsetChannelVoltage) << "[" << getDeviceID() << "] "
#define SCLDBG_ DBG_LOG(CmdMPSsetChannelVoltage) << "[" << getDeviceID() << "] "
#define SCLERR_ ERR_LOG(CmdMPSsetChannelVoltage) << "[" << getDeviceID() << "] "
namespace own = driver::multichannelpowersupply;
namespace c_data =  chaos::common::data;
namespace chaos_batch = chaos::common::batch_command;
using namespace chaos::cu::control_manager;
BATCH_COMMAND_OPEN_DESCRIPTION_ALIAS(driver::multichannelpowersupply::,CmdMPSsetChannelVoltage,CMD_MPS_SETCHANNELVOLTAGE_ALIAS,
	"set the voltage on a specified channel",
	"612b38a6-5366-44db-84e2-72eba97bb3a2")
BATCH_COMMAND_ADD_INT32_PARAM(CMD_MPS_SETCHANNELVOLTAGE_SLOT,"the slot number of the channel",chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_MANDATORY)
BATCH_COMMAND_ADD_INT32_PARAM(CMD_MPS_SETCHANNELVOLTAGE_CHANNEL,"the channel to be set",chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_MANDATORY)
BATCH_COMMAND_ADD_DOUBLE_PARAM(CMD_MPS_SETCHANNELVOLTAGE_VOLTAGE,"the voltage(V) to be set",chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_MANDATORY)
BATCH_COMMAND_CLOSE_DESCRIPTION()


// return the implemented handler
uint8_t own::CmdMPSsetChannelVoltage::implementedHandler(){
	return      AbstractMultiChannelPowerSupplyCommand::implementedHandler()|chaos_batch::HandlerType::HT_Acquisition;
}
// empty set handler
void own::CmdMPSsetChannelVoltage::setHandler(c_data::CDataWrapper *data) {
	AbstractMultiChannelPowerSupplyCommand::setHandler(data);
	this->resolution=getAttributeCache()->getROPtr<double>(DOMAIN_INPUT, "SetResolution");
	this->kindOfGenerator=getAttributeCache()->getROPtr<int32_t>(DOMAIN_INPUT, "GeneratorBehaviour");
	SCLAPP_ << "Set Handler setChannelVoltage ";
	setStateVariableSeverity(StateVariableTypeAlarmCU,"setPoint_not_reached", chaos::common::alarm::MultiSeverityAlarmLevelClear); 
	setStateVariableSeverity(StateVariableTypeAlarmCU,"driver_command_error",chaos::common::alarm::MultiSeverityAlarmLevelClear);
	if(!data || !data->hasKey(CMD_MPS_SETCHANNELVOLTAGE_SLOT))
	{
		SCLERR_ << "slot not present in CDataWrapper";
		metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelError,"slot not present in CDataWrapper" );
		setWorkState(false);
		BC_FAULT_RUNNING_PROPERTY
		return;
	}
	tmp_slot=data->getInt32Value(CMD_MPS_SETCHANNELVOLTAGE_SLOT);

	if(!data || !data->hasKey(CMD_MPS_SETCHANNELVOLTAGE_CHANNEL))
	{
		SCLERR_ << "channel not present in CDataWrapper";
		metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelError,"channel not present in CDataWrapper" );
		setWorkState(false);
		BC_FAULT_RUNNING_PROPERTY
		return;
	}
	tmp_channel=data->getInt32Value(CMD_MPS_SETCHANNELVOLTAGE_CHANNEL);

	if(!data || !data->hasKey(CMD_MPS_SETCHANNELVOLTAGE_VOLTAGE))
	{
		SCLERR_ << "voltage not present in CDataWrapper";
		metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelError,"voltage not present in CDataWrapper" );
		setWorkState(false);
		BC_FAULT_RUNNING_PROPERTY
		return;
	}
	tmp_voltage=data->getDoubleValue(CMD_MPS_SETCHANNELVOLTAGE_VOLTAGE);

	int err=0;
	if ((err=multichannelpowersupply_drv->setChannelVoltage(tmp_slot,tmp_channel,tmp_voltage)) != 0)
	{
		metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelError," command setChannelVoltage not acknowledged");
		setStateVariableSeverity(StateVariableTypeAlarmCU,"driver_command_error",chaos::common::alarm::MultiSeverityAlarmLevelHigh);
	}
	setWorkState(true);
	BC_NORMAL_RUNNING_PROPERTY
}
// empty acquire handler
void own::CmdMPSsetChannelVoltage::acquireHandler() {
	SCLDBG_ << "Acquire Handler setChannelVoltage "; 
	if (this->kindofGenerator == 2)
	{
		if (AbstractMultiChannelPowerSupplyCommand::outputRead() != 0 )
		
		{
			//if (isAVoltageGenerator())
			metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelError," cannot retrieve data  from PowerSupply");
			setStateVariableSeverity(StateVariableTypeAlarmCU,"driver_command_error",chaos::common::alarm::MultiSeverityAlarmLevelHigh);
			BC_FAULT_RUNNING_PROPERTY
			return;
		}
	}


}
// empty correlation handler
void own::CmdMPSsetChannelVoltage::ccHandler() {
	if (this->kindofGenerator != 2)
	{
		BC_END_RUNNING_PROPERTY
	}


	int chanToMonitor=this->getProgressiveChannel(this->tmp_slot,this->tmp_channel);
	if (chanToMonitor < 0)
	{
		metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelError,"command setChannelVoltage bad channel monitoring" );
		setStateVariableSeverity(StateVariableTypeAlarmCU,"driver_command_error",chaos::common::alarm::MultiSeverityAlarmLevelWarning);
		setWorkState(false);
		BC_FAULT_RUNNING_PROPERTY
		return;

	}
	double readValue= chVoltages[chanToMonitor];
	SCLDBG_ << "ALEDEBUG " << readValue << "set value " << this->tmp_voltage << "resolution " << (*this->resolution) ;
	if ((this->resolution==NULL) || (fabs(readValue - this->tmp_voltage) < (*this->resolution)))
	{
		BC_END_RUNNING_PROPERTY;
	}
	
}
// empty timeout handler
bool own::CmdMPSsetChannelVoltage::timeoutHandler() {
	SCLDBG_ << "Timeout Handler setChannelVoltage "; 

	SCLERR_ << "[metric] Setpoint not reached setting channel "<< this->tmp_channel <<" timeout  "  << " in " << " milliseconds";
    setStateVariableSeverity(StateVariableTypeAlarmCU,"setPoint_not_reached", chaos::common::alarm::MultiSeverityAlarmLevelWarning);
	BC_END_RUNNING_PROPERTY
	return false;
}
