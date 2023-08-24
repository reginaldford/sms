// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../../sms.h"

void sm_op_add(){
  sm_op_push4(sm_op_pop()+sm_op_pop());
}

void sm_op_subtract(){
  sm_op_push4(sm_op_pop()-sm_op_pop());
}

void sm_op_multiply(){
  sm_op_push4(sm_op_pop()*sm_op_pop());
}

void sm_op_divide(){
  sm_op_push4(sm_op_pop()/sm_op_pop());
}
