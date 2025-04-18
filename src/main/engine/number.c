// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"

// function ptr arrays are defined in sm_global.c
extern sm_object *(*sm_add_functions[])(sm_object *o0, sm_object *o1);
extern sm_object *(*sm_minus_functions[])(sm_object *o0, sm_object *o1);
extern sm_object *(*sm_times_functions[])(sm_object *o0, sm_object *o1);
extern sm_object *(*sm_divide_functions[])(sm_object *o0, sm_object *o1);

// minus operations
sm_object *sm_add() {
  sm_object *o0 = sm_pop();
  sm_object *o1 = sm_pop();
  return sm_add_functions[o0->my_type * 2 + o1->my_type](o0, o1);
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
  uint8_t b = ((sm_ui8 *)o1)->value;
  return (sm_object *)sm_new_f64(a + b);
}

// Minus operations
sm_object *sm_minus() {
  sm_object *o0 = sm_pop();
  sm_object *o1 = sm_pop();
  return sm_minus_functions[o0->my_type * 2 + o1->my_type](o0, o1);
}

sm_object *sm_minus_uint8_and_uint8(sm_object *o0, sm_object *o1) {
  uint8_t a = ((sm_ui8 *)o0)->value;
  uint8_t b = ((sm_ui8 *)o1)->value;
  return (sm_object *)sm_new_ui8(b - a);
}

sm_object *sm_minus_uint8_and_f64(sm_object *o0, sm_object *o1) {
  uint8_t a = ((sm_ui8 *)o0)->value;
  double  b = ((sm_f64 *)o1)->value;
  return (sm_object *)sm_new_f64(b - a);
}

sm_object *sm_minus_f64_and_f64(sm_object *o0, sm_object *o1) {
  double a = ((sm_f64 *)o0)->value;
  double b = ((sm_f64 *)o1)->value;
  return (sm_object *)sm_new_f64(b - a);
}

sm_object *sm_minus_f64_and_uint8(sm_object *o0, sm_object *o1) {
  double  a = ((sm_f64 *)o0)->value;
  uint8_t b = ((sm_ui8 *)o1)->value;
  return (sm_object *)sm_new_f64(b - a);
}
