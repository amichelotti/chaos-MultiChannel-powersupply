/*
MultiChannelPowerSupplyConstants.h
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
#ifndef MultiChannelPowerSupply_MultiChannelPowerSupplyConstants_h
#define MultiChannelPowerSupply_MultiChannelPowerSupplyConstants_h
namespace driver {
	namespace multichannelpowersupply {
		#define TROW_ERROR(n,m,d) throw chaos::CException(n, m, d);
		#define LOG_TAIL(n) "[" << #n << "] - " << getDeviceID() << " - [" << getUID() << "] - " 
		const char* const CMD_MPS_DEFAULT_ALIAS = "Default";
		const char* const CMD_MPS_UPDATEHV_ALIAS = "UpdateHV";
		const char* const CMD_MPS_UPDATEHV_CRATEDATA = "crateData";
		const char* const CMD_MPS_GETSLOTCONFIGURATION_ALIAS = "getSlotConfiguration";
		const char* const CMD_MPS_GETSLOTCONFIGURATION_SLOTNUMBERLIST = "slotNumberList";
		const char* const CMD_MPS_GETSLOTCONFIGURATION_CHANNELSPERSLOT = "channelsPerSlot";
		const char* const CMD_MPS_SETCHANNELVOLTAGE_ALIAS = "setChannelVoltage";
		const char* const CMD_MPS_SETCHANNELVOLTAGE_SLOT = "slot";
		const char* const CMD_MPS_SETCHANNELVOLTAGE_CHANNEL = "channel";
		const char* const CMD_MPS_SETCHANNELVOLTAGE_VOLTAGE = "voltage";
		const char* const CMD_MPS_SETCHANNELCURRENT_ALIAS = "setChannelCurrent";
		const char* const CMD_MPS_SETCHANNELCURRENT_SLOT = "slot";
		const char* const CMD_MPS_SETCHANNELCURRENT_CHANNEL = "channel";
		const char* const CMD_MPS_SETCHANNELCURRENT_CURRENT = "current";
		const char* const CMD_MPS_GETCHANNELPARAMETERSDESCRIPTION_ALIAS = "getChannelParametersDescription";
		const char* const CMD_MPS_GETCHANNELPARAMETERSDESCRIPTION_OUTJSONSTRING = "outJSONString";
		const char* const CMD_MPS_SETCHANNELPARAMETER_ALIAS = "setChannelParameter";
		const char* const CMD_MPS_SETCHANNELPARAMETER_SLOT = "slot";
		const char* const CMD_MPS_SETCHANNELPARAMETER_CHANNEL = "channel";
		const char* const CMD_MPS_SETCHANNELPARAMETER_PARAMNAME = "paramName";
		const char* const CMD_MPS_SETCHANNELPARAMETER_PARAMVALUE = "paramValue";
		const char* const CMD_MPS_POWERON_ALIAS = "PowerOn";
		const char* const CMD_MPS_POWERON_SLOT = "slot";
		const char* const CMD_MPS_POWERON_CHANNEL = "channel";
		const char* const CMD_MPS_POWERON_ONSTATE = "onState";
		const char* const CMD_MPS_MAINUNITPOWERON_ALIAS = "MainUnitPowerOn";
		const char* const CMD_MPS_MAINUNITPOWERON_ON_STATE = "on_state";
		const char* const CMD_MPS_GETMAINSTATUS_ALIAS = "getMainStatus";
		const char* const CMD_MPS_GETMAINSTATUS_STATUS = "status";
		const char* const CMD_MPS_GETMAINSTATUS_DESCR = "descr";
		const char* const CMD_MPS_GETMAINALARMS_ALIAS = "getMainAlarms";
		const char* const CMD_MPS_GETMAINALARMS_ALARMS = "alarms";
		const char* const CMD_MPS_GETMAINALARMS_DESCR = "descr";
		#define DEFAULT_COMMAND_TIMEOUT_MS   10000
	}
}
#endif
