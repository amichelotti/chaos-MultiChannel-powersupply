/*
CmdMPSPowerOn.h
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
#ifndef __MultiChannelPowerSupply__CmdMPSPowerOn_h__
#define __MultiChannelPowerSupply__CmdMPSPowerOn_h__
#include "AbstractMultiChannelPowerSupplyCommand.h"
namespace c_data = chaos::common::data;
namespace ccc_slow_command = chaos::cu::control_manager::slow_command;
namespace driver {
	namespace multichannelpowersupply {
		DEFINE_BATCH_COMMAND_CLASS(CmdMPSPowerOn,AbstractMultiChannelPowerSupplyCommand) {
			//implemented handler
			uint8_t implementedHandler();
			//initial set handler
			void setHandler(c_data::CDataWrapper *data);
			//custom acquire handler
			void acquireHandler();
			//correlation and commit handler
			void ccHandler();
			//manage the timeout 
			bool timeoutHandler();
			int32_t tmp_onState;
			int32_t tmp_channel;
			int32_t tmp_slot;
			double lastValue;
			const double *resolution;
		};
	}
}
#endif
