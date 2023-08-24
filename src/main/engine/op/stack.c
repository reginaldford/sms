// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../../sms.h"

sm_bstack sms_bstack;

void sm_op_push(uint8_t value){
  sms_bstack.data[++sms_bstack.top]=value;
}

uint8_t sm_op_pop(){
  uint8_t value = sms_bstack.data[sms_bstack.top--];
  return value;
}

void sm_op_push2(uint16_t value){
  sms_bstack.data[++sms_bstack.top]=value;
}

uint16_t sm_op_pop2(){
  uint16_t value = sms_bstack.data[sms_bstack.top--];
  return value;
}

void sm_op_push4(uint32_t value){
  ((uint32_t*)sms_bstack.data)[++sms_bstack.top]=value;
}

uint32_t sm_op_pop4(){
  uint32_t value = sms_bstack.data[sms_bstack.top--];
  return value;
}

void sm_op_push8(uint64_t value){
  ((uint64_t*)sms_bstack.data)[++sms_bstack.top]=value;
}

uint64_t sm_op_pop8(){
  uint64_t value = sms_bstack.data[sms_bstack.top--];
  return value;
}

