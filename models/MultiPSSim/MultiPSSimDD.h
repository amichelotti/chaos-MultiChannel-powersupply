/*
MultiPSSimDD.h
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
#ifndef __driver_MultiPSSimDD_h__
#define __driver_MultiPSSimDD_h__
#ifndef DRLAPP
#define DRLAPP LAPP_ << "[MultiPSSimDD]"
#endif
#include <chaos/cu_toolkit/driver_manager/driver/AbstractDriverPlugin.h>
#include <driver/MultiChannelPowerSupply/core/ChaosMultiChannelPowerSupplyDD.h>
DEFINE_CU_DRIVER_DEFINITION_PROTOTYPE(MultiPSSimDD)
namespace cu_driver = chaos::cu::driver_manager::driver;
namespace chaos {
	namespace driver {
		namespace multichannelpowersupply{
			class MultiPSSimDD: public ChaosMultiChannelPowerSupplyDD{
				void driverInit(const char *initParameter);
				void driverInit(const chaos::common::data::CDataWrapper& json);
			public:
				MultiPSSimDD();
				~MultiPSSimDD();
			};//end class
		} //namespace multichannelpowersupply
	} //namespace driver
} //namespace chaos
#endif
