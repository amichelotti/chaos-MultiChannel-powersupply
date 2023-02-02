/*
ChaosMultiChannelPowerSupplyDD.cpp
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
#include "ChaosMultiChannelPowerSupplyDD.h"
#include <string>
#include <regex>
#include <chaos/cu_toolkit/driver_manager/driver/AbstractDriverPlugin.h>
// including interface
#include "driver/MultiChannelPowerSupply/core/ChaosMultiChannelPowerSupplyInterface.h"
#define ACLAPP	LAPP_ << "[ChaosMultiChannelPowerSupplyDD] "
#define ACDBG	LDBG_ << "[ChaosMultiChannelPowerSupplyDD] "
#define ACERR	LERR_ << "[ChaosMultiChannelPowerSupplyDD] "
using namespace chaos::driver::multichannelpowersupply;
//default constructor definition
DEFAULT_CU_DRIVER_PLUGIN_CONSTRUCTOR_WITH_NS(chaos::driver::multichannelpowersupply, ChaosMultiChannelPowerSupplyDD) {
	devicedriver = NULL;
}
ChaosMultiChannelPowerSupplyDD::~ChaosMultiChannelPowerSupplyDD() {
}
void ChaosMultiChannelPowerSupplyDD::driverDeinit() {
	 if(devicedriver) {
		delete devicedriver;
	}
	devicedriver = NULL;
}
cu_driver::MsgManagmentResultType::MsgManagmentResult ChaosMultiChannelPowerSupplyDD::execOpcode(cu_driver::DrvMsgPtr cmd){
	 cu_driver::MsgManagmentResultType::MsgManagmentResult result = cu_driver::MsgManagmentResultType::MMR_EXECUTED;
	multichannelpowersupply_iparams_t *in = (multichannelpowersupply_iparams_t *)cmd->inputData;
	multichannelpowersupply_oparams_t *out = (multichannelpowersupply_oparams_t *)cmd->resultData;
	switch(cmd->opcode){
		case OP_UPDATEHV: {
			out->result=devicedriver->UpdateHV(out->stringE1);
			ACDBG << "Sent to driver command UpdateHV result is " << out->result;
			} break;
		case OP_GETSLOTCONFIGURATION: {
			out->result=devicedriver->getSlotConfiguration(out->vector_int32_t_E1,out->vector_int32_t_E2);
			ACDBG << "Sent to driver command getSlotConfiguration result is " << out->result;
			} break;
		case OP_SETCHANNELVOLTAGE: {
			out->result=devicedriver->setChannelVoltage(in->int32_t1,in->int32_t2,in->double1);
			ACDBG << "Sent to driver command setChannelVoltage result is " << out->result;
			} break;
		case OP_SETCHANNELCURRENT: {
			out->result=devicedriver->setChannelCurrent(in->int32_t1,in->int32_t2,in->double1);
			ACDBG << "Sent to driver command setChannelCurrent result is " << out->result;
			} break;
		case OP_GETCHANNELPARAMETERSDESCRIPTION: {
			out->result=devicedriver->getChannelParametersDescription(out->stringE1);
			ACDBG << "Sent to driver command getChannelParametersDescription result is " << out->result;
			} break;
		case OP_SETCHANNELPARAMETER: {
			out->result=devicedriver->setChannelParameter(in->int32_t1,in->int32_t2,in->string1,in->string2);
			ACDBG << "Sent to driver command setChannelParameter result is " << out->result;
			} break;
		case OP_POWERON: {
			out->result=devicedriver->PowerOn(in->int32_t1,in->int32_t2,in->int32_t3);
			ACDBG << "Sent to driver command PowerOn result is " << out->result;
			} break;
		case OP_MAINUNITPOWERON: {
			out->result=devicedriver->MainUnitPowerOn(in->int32_t1);
			ACDBG << "Sent to driver command MainUnitPowerOn result is " << out->result;
			} break;
		case OP_GETMAINSTATUS: {
			out->result=devicedriver->getMainStatus(out->int32_tE1,out->stringE1);
			
			} break;
		case OP_GETMAINALARMS: {
			out->result=devicedriver->getMainAlarms(out->int64_tE1,out->stringE1);
			
			} break;
	}
	return result;
}
