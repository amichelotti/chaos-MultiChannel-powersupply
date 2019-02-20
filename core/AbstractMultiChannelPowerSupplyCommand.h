/*
AbstractMultiChannelPowerSupplyCommand.h
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
#ifndef __AbstractMultiChannelPowerSupplyCommand__
#define __AbstractMultiChannelPowerSupplyCommand__
#include "MultiChannelPowerSupplyConstants.h"
#include <driver/MultiChannelPowerSupply/core/ChaosMultiChannelPowerSupplyInterface.h>
#include <chaos/cu_toolkit/control_manager/slow_command/SlowCommand.h>

namespace c_data = chaos::common::data;
namespace ccc_slow_command = chaos::cu::control_manager::slow_command;
namespace driver {
	namespace multichannelpowersupply{
		class AbstractMultiChannelPowerSupplyCommand: public ccc_slow_command::SlowCommand {
		public:
			AbstractMultiChannelPowerSupplyCommand();
			~AbstractMultiChannelPowerSupplyCommand();
		protected: 
			//common members
			int32_t	*o_status_id;
			//uint64_t	*o_alarms;
			//reference of the chaos abstraction of driver
			chaos::driver::multichannelpowersupply::ChaosMultiChannelPowerSupplyInterface *multichannelpowersupply_drv;
			//implemented handler
			uint8_t implementedHandler();
			void ccHandler();
			void setHandler(c_data::CDataWrapper *data);
			void setWorkState(bool working);
			int32_t outputRead();
			double* chVoltages;
			double* chCurrents;
			int64_t* chStatus;
			int64_t* chAlarms;
			const char* paramToShow;
			char* auxiliaryAvailable;
			std::vector<std::string> auxParamList;
			std::vector<std::string> auxParamTypeList;
			bool alreadyLoggedNotRetrieving;
			bool alreadyLoggedWrongJSON;
			bool alreadyLoggedJSONFormat;
			bool alreadyLoggedUnknownFormatData;
			bool alreadyLoggedAuxParamNotFound;
		public:
			std::string getTypeOfAuxParam(std::string par);
			int32_t getProgressiveChannel(int32_t slot, int32_t channel);
			#define  JSON_FORMAT "UpdateHW must return  a JSON VECTOR string  with a component for each channel. Each component is an object with at least the following key: \
			VMon (double), IMon (double), status (int64_t), alarm(int64_t)"
		};// AbstractMultiChannelPowerSupplyCommand
	}// multichannelpowersupply
}
#endif
