/*
ChaosMultiChannelPowerSupplyInterface.h
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
#ifndef __ChaosMultiChannelPowerSupplyInterface__
#define __ChaosMultiChannelPowerSupplyInterface__
#include <iostream>
#include <chaos/cu_toolkit/driver_manager/driver/DriverTypes.h>
#include <chaos/cu_toolkit/driver_manager/driver/DriverAccessor.h>
#include <common/debug/core/debug.h>
#include <stdint.h>
#include <common/MultiChannelPowerSupply/core/AbstractMultiChannelPowerSupply.h>
namespace chaos_driver=::chaos::cu::driver_manager::driver;
namespace chaos {
	namespace driver {
		#define MAX_STR_SIZE 256
		namespace multichannelpowersupply {
			typedef enum {
				OP_UPDATEHV,
				OP_GETSLOTCONFIGURATION,
				OP_SETCHANNELVOLTAGE,
				OP_SETCHANNELCURRENT,
				OP_GETCHANNELPARAMETERSDESCRIPTION,
				OP_SETCHANNELPARAMETER,
				OP_POWERON
			} ChaosMultiChannelPowerSupplyOpcode;
			typedef struct {
				uint32_t timeout;
				int32_t int32_t1;
				int32_t int32_t2;
				int32_t int32_t3;
				double double1;
				std::string string1;
				std::string string2;
			} multichannelpowersupply_iparams_t;
			typedef struct {
				int64_t int64_t1;
				int32_t result;
				std::string  stringE1;
				std::vector<int32_t>  vector_int32_t_E1;
				std::vector<int32_t>  vector_int32_t_E2;
			} multichannelpowersupply_oparams_t;
			//wrapper interface
			class ChaosMultiChannelPowerSupplyInterface:public ::common::multichannelpowersupply::AbstractMultiChannelPowerSupply {
				protected:
				chaos_driver::DrvMsg message;
				public: 
				ChaosMultiChannelPowerSupplyInterface(chaos_driver::DriverAccessor*_accessor):accessor(_accessor){};
				chaos_driver::DriverAccessor* accessor;
				int UpdateHV(std::string& crateData);
				int getSlotConfiguration(std::vector<int32_t>& slotNumberList,std::vector<int32_t>& channelsPerSlot);
				int setChannelVoltage(int32_t slot,int32_t channel,double voltage);
				int setChannelCurrent(int32_t slot,int32_t channel,double current);
				int getChannelParametersDescription(std::string& outJSONString);
				int setChannelParameter(int32_t slot,int32_t channel,std::string paramName,std::string paramValue);
				int PowerOn(int32_t slot,int32_t channel,int32_t onState);
			};
		}
	}//driver
}//chaos
namespace chaos_multichannelpowersupply_dd = chaos::driver::multichannelpowersupply;
#endif
