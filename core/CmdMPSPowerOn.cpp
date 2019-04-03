/*
CmdMPSPowerOn.cpp
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
#include "CmdMPSPowerOn.h"

#include <cmath>
#include  <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <sstream>
#define SCLAPP_ INFO_LOG(CmdMPSPowerOn) << "[" << getDeviceID() << "] "
#define SCLDBG_ DBG_LOG(CmdMPSPowerOn) << "[" << getDeviceID() << "] "
#define SCLERR_ ERR_LOG(CmdMPSPowerOn) << "[" << getDeviceID() << "] "
namespace own = driver::multichannelpowersupply;
namespace c_data =  chaos::common::data;
namespace chaos_batch = chaos::common::batch_command;
using namespace chaos::cu::control_manager;
BATCH_COMMAND_OPEN_DESCRIPTION_ALIAS(driver::multichannelpowersupply::,CmdMPSPowerOn,CMD_MPS_POWERON_ALIAS,
	"turn on or off a channel",
	"320d16c1-f539-4ec9-a1de-aff29dbb06c3")
BATCH_COMMAND_ADD_INT32_PARAM(CMD_MPS_POWERON_SLOT,"the slot of the channel to switch. (-1 for all slot)",chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_MANDATORY)
BATCH_COMMAND_ADD_INT32_PARAM(CMD_MPS_POWERON_CHANNEL,"the channel to switch. (-1 for all channel in the slot)",chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_MANDATORY)
BATCH_COMMAND_ADD_INT32_PARAM(CMD_MPS_POWERON_ONSTATE,"OFF = 0 ; ON = 1",chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_MANDATORY)
BATCH_COMMAND_CLOSE_DESCRIPTION()


// return the implemented handler
uint8_t own::CmdMPSPowerOn::implementedHandler(){
	return      AbstractMultiChannelPowerSupplyCommand::implementedHandler()|chaos_batch::HandlerType::HT_Acquisition;
}
// empty set handler
void own::CmdMPSPowerOn::setHandler(c_data::CDataWrapper *data) {
	AbstractMultiChannelPowerSupplyCommand::setHandler(data);
	SCLAPP_ << "Set Handler PowerOn "; 
	this->resolution=getAttributeCache()->getROPtr<double>(DOMAIN_INPUT, "SetResolution");
	lastValue=-99999;
	setStateVariableSeverity(StateVariableTypeAlarmCU,"driver_command_error",chaos::common::alarm::MultiSeverityAlarmLevelClear);
	if(!data || !data->hasKey(CMD_MPS_POWERON_SLOT))
	{
		SCLERR_ << "slot not present in CDataWrapper";
		metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelError,"slot not present in CDataWrapper" );
		setWorkState(false);
		BC_FAULT_RUNNING_PROPERTY
		return;
	}
	tmp_slot=data->getInt32Value(CMD_MPS_POWERON_SLOT);

	if(!data || !data->hasKey(CMD_MPS_POWERON_CHANNEL))
	{
		SCLERR_ << "channel not present in CDataWrapper";
		metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelError,"channel not present in CDataWrapper" );
		setWorkState(false);
		BC_FAULT_RUNNING_PROPERTY
		return;
	}
	 tmp_channel=data->getInt32Value(CMD_MPS_POWERON_CHANNEL);

	if(!data || !data->hasKey(CMD_MPS_POWERON_ONSTATE))
	{
		SCLERR_ << "onState not present in CDataWrapper";
		metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelError,"onState not present in CDataWrapper" );
		setWorkState(false);
		BC_FAULT_RUNNING_PROPERTY
		return;
	}
	 tmp_onState=data->getInt32Value(CMD_MPS_POWERON_ONSTATE);

	int err=0;
	if ((err=multichannelpowersupply_drv->PowerOn(tmp_slot,tmp_channel,tmp_onState)) != 0)
	{
		metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelError," command PowerOn not acknowledged");
		setStateVariableSeverity(StateVariableTypeAlarmCU,"driver_command_error",chaos::common::alarm::MultiSeverityAlarmLevelHigh);
	}
	setWorkState(true);
	BC_NORMAL_RUNNING_PROPERTY
}
// empty acquire handler
void own::CmdMPSPowerOn::acquireHandler() {
	AbstractMultiChannelPowerSupplyCommand::outputRead();
	
}
// empty correlation handler
void own::CmdMPSPowerOn::ccHandler() {
	SCLDBG_ << "CC Handler PowerOn";
	if (*kindOfGenerator != ::common::multichannelpowersupply::MPS_NOT_SPECIFIED)
	{
		if (tmp_onState == 1)
		{
			double derivative;
			int chanToMonitor=this->getProgressiveChannel(this->tmp_slot,this->tmp_channel);
			if (*kindOfGenerator == ::common::multichannelpowersupply::MPS_CURRENT_GENERATOR)
			{
				derivative=lastValue - chCurrents[chanToMonitor];
				if (derivative < (*this->resolution))
				{
					setVals[chanToMonitor]=chCurrents[chanToMonitor];
					getAttributeCache()->setInputDomainAsChanged();
					getAttributeCache()->setOutputDomainAsChanged();
					BC_END_RUNNING_PROPERTY;
				}
				else
					lastValue=chCurrents[chanToMonitor];
			}
			else
			{
				derivative=std::fabs(lastValue - chVoltages[chanToMonitor]);

				if (derivative < (*this->resolution))
				{
					setVals[chanToMonitor]=chVoltages[chanToMonitor];
					getAttributeCache()->setInputDomainAsChanged();
					getAttributeCache()->setOutputDomainAsChanged();
					BC_END_RUNNING_PROPERTY;
				}
				else/* code */
					lastValue=chVoltages[chanToMonitor];
			}
			sleep(1);
		}
		else
		{
			BC_END_RUNNING_PROPERTY;
		}
	}
}
// empty timeout handler
bool own::CmdMPSPowerOn::timeoutHandler() {
	SCLDBG_ << "Timeout Handler PowerOn "; 
	return false;
}
