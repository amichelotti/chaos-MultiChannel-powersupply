/*
ChaosMultiChannelPowerSupplyInterface.cpp
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
#include "ChaosMultiChannelPowerSupplyInterface.h"
using namespace chaos::driver::multichannelpowersupply;
#define PREPARE_OP_RET_INT_TIMEOUT(op,tim) \
multichannelpowersupply_oparams_t ret;\
multichannelpowersupply_iparams_t idata;\
message.opcode = op; \
message.inputData=(void*)&idata;\
idata.timeout=tim;\
message.inputDataLength=sizeof(multichannelpowersupply_iparams_t);\
message.resultDataLength=sizeof(multichannelpowersupply_oparams_t);\
message.resultData = (void*)&ret;\

#define WRITE_OP_TIM(op,timeout) \
PREPARE_OP_RET_INT_TIMEOUT(op,timeout); \
accessor->send(&message);\
return ret.result;

#define WRITE_OP_INT32_T_INT32_T_DOUBLE_TIM(op,VAR_int32_t1,VAR_int32_t2,VAR_double1,timeout)\
PREPARE_OP_RET_INT_TIMEOUT(op,timeout); \
idata.int32_t1=VAR_int32_t1;\
idata.int32_t2=VAR_int32_t2;\
idata.double1=VAR_double1;\
accessor->send(&message);\
return ret.result;

#define WRITE_OP_INT32_T_INT32_T_STRING_STRING_TIM(op,VAR_int32_t1,VAR_int32_t2,VAR_string1,VAR_string2,timeout)\
PREPARE_OP_RET_INT_TIMEOUT(op,timeout); \
idata.int32_t1=VAR_int32_t1;\
idata.int32_t2=VAR_int32_t2;\
idata.string1=VAR_string1;\
idata.string2=VAR_string2;\
DPRINT("ALEDEBUG:before accessor send");\
accessor->send(&message);\
return ret.result;

#define WRITE_OP_INT32_T_INT32_T_INT32_T_TIM(op,VAR_int32_t1,VAR_int32_t2,VAR_int32_t3,timeout)\
PREPARE_OP_RET_INT_TIMEOUT(op,timeout); \
idata.int32_t1=VAR_int32_t1;\
idata.int32_t2=VAR_int32_t2;\
idata.int32_t3=VAR_int32_t3;\
accessor->send(&message);\
return ret.result;

#define WRITE_OP_INT32_T_TIM(op,VAR_int32_t1,timeout)\
PREPARE_OP_RET_INT_TIMEOUT(op,timeout); \
idata.int32_t1=VAR_int32_t1;\
accessor->send(&message);\
return ret.result;

#define READ_OP_STRING_TIM(op,VAR_stringE1,timeout)\
PREPARE_OP_RET_INT_TIMEOUT(op,timeout); \
accessor->send(&message);\
VAR_stringE1=ret.stringE1;\
return ret.result;

#define READ_OP_VECTOR_INT32_T__VECTOR_INT32_T__TIM(op,VAR_vector_int32_t_E1,VAR_vector_int32_t_E2,timeout)\
PREPARE_OP_RET_INT_TIMEOUT(op,timeout); \
accessor->send(&message);\
VAR_vector_int32_t_E1=ret.vector_int32_t_E1;\
VAR_vector_int32_t_E2=ret.vector_int32_t_E2;\
return ret.result;

#define READ_OP_INT32_T_STRING_TIM(op,VAR_int32_tE1,VAR_stringE1,timeout)\
PREPARE_OP_RET_INT_TIMEOUT(op,timeout); \
accessor->send(&message);\
VAR_int32_tE1=ret.int32_tE1;\
VAR_stringE1=ret.stringE1;\
return ret.result;

#define READ_OP_INT64_T_STRING_TIM(op,VAR_int64_tE1,VAR_stringE1,timeout)\
PREPARE_OP_RET_INT_TIMEOUT(op,timeout); \
accessor->send(&message);\
VAR_int64_tE1=ret.int64_tE1;\
VAR_stringE1=ret.stringE1;\
return ret.result;

int ChaosMultiChannelPowerSupplyInterface::setChannelVoltage(int32_t slot,int32_t channel,double voltage) {
	WRITE_OP_INT32_T_INT32_T_DOUBLE_TIM(OP_SETCHANNELVOLTAGE,slot,channel,voltage,0);
} 
int ChaosMultiChannelPowerSupplyInterface::setChannelCurrent(int32_t slot,int32_t channel,double current) {
	WRITE_OP_INT32_T_INT32_T_DOUBLE_TIM(OP_SETCHANNELCURRENT,slot,channel,current,0);
} 
int ChaosMultiChannelPowerSupplyInterface::setChannelParameter(int32_t slot,int32_t channel,std::string paramName,std::string paramValue) {
	WRITE_OP_INT32_T_INT32_T_STRING_STRING_TIM(OP_SETCHANNELPARAMETER,slot,channel,paramName,paramValue,0);
} 
int ChaosMultiChannelPowerSupplyInterface::PowerOn(int32_t slot,int32_t channel,int32_t onState) {
	WRITE_OP_INT32_T_INT32_T_INT32_T_TIM(OP_POWERON,slot,channel,onState,0);
} 
int ChaosMultiChannelPowerSupplyInterface::MainUnitPowerOn(int32_t on_state) {
	WRITE_OP_INT32_T_TIM(OP_MAINUNITPOWERON,on_state,0);
} 
int ChaosMultiChannelPowerSupplyInterface::UpdateHV(std::string& crateData) {
	READ_OP_STRING_TIM(OP_UPDATEHV,crateData,0);
} 
int ChaosMultiChannelPowerSupplyInterface::getSlotConfiguration(std::vector<int32_t>& slotNumberList,std::vector<int32_t>& channelsPerSlot) {
	READ_OP_VECTOR_INT32_T__VECTOR_INT32_T__TIM(OP_GETSLOTCONFIGURATION,slotNumberList,channelsPerSlot,0);
} 
int ChaosMultiChannelPowerSupplyInterface::getChannelParametersDescription(std::string& outJSONString) {
	READ_OP_STRING_TIM(OP_GETCHANNELPARAMETERSDESCRIPTION,outJSONString,0);
} 
int ChaosMultiChannelPowerSupplyInterface::getMainStatus(int32_t& status,std::string& descr) {
	READ_OP_INT32_T_STRING_TIM(OP_GETMAINSTATUS,status,descr,0);
} 
int ChaosMultiChannelPowerSupplyInterface::getMainAlarms(int64_t& alarms,std::string& descr) {
	READ_OP_INT64_T_STRING_TIM(OP_GETMAINALARMS,alarms,descr,0);
} 
