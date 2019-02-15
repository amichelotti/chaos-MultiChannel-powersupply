/*
MultiPSSimDD.cpp
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
#include "MultiPSSimDD.h"
#include "driver/MultiChannelPowerSupply/core/ChaosMultiChannelPowerSupplyInterface.h"
#include <common/MultiChannelPowerSupply/models/MultiPSSim/MultiPSSim.h>
#include <common/misc/driver/ConfigDriverMacro.h>
OPEN_CU_DRIVER_PLUGIN_CLASS_DEFINITION(MultiPSSimDD,1.0.0, chaos::driver::multichannelpowersupply::MultiPSSimDD)
REGISTER_CU_DRIVER_PLUGIN_CLASS_INIT_ATTRIBUTE(chaos::driver::multichannelpowersupply::MultiPSSimDD, http_address/dnsname:port)
CLOSE_CU_DRIVER_PLUGIN_CLASS_DEFINITION
OPEN_REGISTER_PLUGIN
REGISTER_PLUGIN(chaos::driver::multichannelpowersupply::MultiPSSimDD)
CLOSE_REGISTER_PLUGIN
chaos::driver::multichannelpowersupply::MultiPSSimDD::MultiPSSimDD() {
	devicedriver = NULL;
}
chaos::driver::multichannelpowersupply::MultiPSSimDD::~MultiPSSimDD() {
}
#ifdef CHAOS
void chaos::driver::multichannelpowersupply::MultiPSSimDD::driverInit(const chaos::common::data::CDataWrapper& json) {
	DRLAPP<< "Initializing MultiPSSimDD HL Driver with CDataWrapper "<<std::endl;
	if (devicedriver) {
		throw chaos::CException(1,"Already Initialized ","MultiPSSimDD::driverInit");
	}
	devicedriver= new ::common::multichannelpowersupply::models::MultiPSSim(json);
	if (devicedriver==NULL)
	{
		throw chaos::CException(1,"Cannot allocate resources for MultiPSSim","MultiPSSimDD::driverInit");
	}
}
#endif
void chaos::driver::multichannelpowersupply::MultiPSSimDD::driverInit(const char* initParameter) {
	DRLAPP<< "Initializing MultiPSSimDD HL Driver with string "<< initParameter <<std::endl;
	if (devicedriver) {
		throw chaos::CException(1,"Already Initialized ","MultiPSSimDD::driverInit");
	}
	devicedriver= new ::common::multichannelpowersupply::models::MultiPSSim(initParameter);
	if (devicedriver==NULL)
	{
		throw chaos::CException(1,"Cannot allocate resources for MultiPSSim","MultiPSSimDD::driverInit");
	}
}
