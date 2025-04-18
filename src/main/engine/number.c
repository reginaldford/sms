// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"
extern sm_object *(*number_funs[])(sm_object *o0, sm_object *o1);

sm_object *sm_add() {
  sm_object *o0 = sm_pop();
  sm_object *o1 = sm_pop();
  return number_funs[o0->my_type * 2 + o1->my_type](o0, o1);
}

sm_object *sm_add_uint8_and_uint8(sm_object *o0, sm_object *o1) {
  uint8_t a = ((sm_ui8 *)o0)->value;
  uint8_t b = ((sm_ui8 *)o1)->value;
  return (sm_object *)sm_new_ui8(a + b);
}

sm_object *sm_add_uint8_and_f64(sm_object *o0, sm_object *o1) {
  uint8_t a = ((sm_ui8 *)o0)->value;
  double  b = ((sm_f64 *)o1)->value;
  return (sm_object *)sm_new_f64(a + b);
}

sm_object *sm_add_f64_and_f64(sm_object *o0, sm_object *o1) {
  double a = ((sm_f64 *)o0)->value;
  double b = ((sm_f64 *)o1)->value;
  return (sm_object *)sm_new_f64(a + b);
}

sm_object *sm_add_f64_and_uint8(sm_object *o0, sm_object *o1) {
  double  a = ((sm_f64 *)o0)->value;
  uint8_t b = ((sm_f64 *)o1)->value;
  return (sm_object *)sm_new_f64(a + b);
}
