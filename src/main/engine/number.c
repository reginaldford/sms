// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"

sm_object *sm_add_uint8_and_uint8() {
  uint8_t a = ((sm_ui8 *)sm_pop())->value;
  uint8_t b = ((sm_ui8 *)sm_pop())->value;
  return (sm_object *)sm_new_ui8(a + b);
}

sm_object *sm_add_uint8_and_f64() {
  uint8_t a = ((sm_ui8 *)sm_pop())->value;
  double  b = ((sm_f64 *)sm_pop())->value;
  return (sm_object *)sm_new_f64(a + b);
}

sm_object *sm_add_f64_and_f64() {
  uint8_t a = ((sm_f64 *)sm_pop())->value;
  double  b = ((sm_f64 *)sm_pop())->value;
  return (sm_object *)sm_new_f64(a + b);
}

sm_object *sm_add_f64_and_uint8() {
  uint8_t a = ((sm_f64 *)sm_pop())->value;
  double  b = ((sm_f64 *)sm_pop())->value;
  return (sm_object *)sm_new_f64(a + b);
}
