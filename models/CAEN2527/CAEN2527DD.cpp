/*
CAEN2527DD.cpp
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
#include "CAEN2527DD.h"
#include "driver/MultiChannelPowerSupply/core/ChaosMultiChannelPowerSupplyInterface.h"
#include <common/MultiChannelPowerSupply/models/CAEN2527/CAEN2527.h>
#include <common/misc/driver/ConfigDriverMacro.h>
OPEN_CU_DRIVER_PLUGIN_CLASS_DEFINITION(CAEN2527DD,1.0.0, chaos::driver::multichannelpowersupply::CAEN2527DD)
REGISTER_CU_DRIVER_PLUGIN_CLASS_INIT_ATTRIBUTE(chaos::driver::multichannelpowersupply::CAEN2527DD, http_address/dnsname:port)
CLOSE_CU_DRIVER_PLUGIN_CLASS_DEFINITION
OPEN_REGISTER_PLUGIN
REGISTER_PLUGIN(chaos::driver::multichannelpowersupply::CAEN2527DD)
CLOSE_REGISTER_PLUGIN
chaos::driver::multichannelpowersupply::CAEN2527DD::CAEN2527DD() {
	devicedriver = NULL;
}
chaos::driver::multichannelpowersupply::CAEN2527DD::~CAEN2527DD() {
}
#ifdef CHAOS
void chaos::driver::multichannelpowersupply::CAEN2527DD::driverInit(const chaos::common::data::CDataWrapper& json) {
	DRLAPP<< "Initializing CAEN2527DD HL Driver with CDataWrapper "<<std::endl;
	if (devicedriver) {
		throw chaos::CException(1,"Already Initialized ","CAEN2527DD::driverInit");
	}
	devicedriver= new ::common::multichannelpowersupply::models::CAEN2527(json);
	if (devicedriver==NULL)
	{
		throw chaos::CException(1,"Cannot allocate resources for CAEN2527","CAEN2527DD::driverInit");
	}
}
#endif
void chaos::driver::multichannelpowersupply::CAEN2527DD::driverInit(const char* initParameter) {
	DRLAPP<< "Initializing CAEN2527DD HL Driver with string "<< initParameter <<std::endl;
	if (devicedriver) {
		throw chaos::CException(1,"Already Initialized ","CAEN2527DD::driverInit");
	}
	devicedriver= new ::common::multichannelpowersupply::models::CAEN2527(initParameter);
	if (devicedriver==NULL)
	{
		throw chaos::CException(1,"Cannot allocate resources for CAEN2527","CAEN2527DD::driverInit");
	}
}
