// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"

// function ptr arrays are defined in sm_global.c
extern sm_object *(*sm_add_functions[])(sm_object *o0, sm_object *o1);
extern sm_object *(*sm_minus_functions[])(sm_object *o0, sm_object *o1);
extern sm_object *(*sm_times_functions[])(sm_object *o0, sm_object *o1);
extern sm_object *(*sm_divide_functions[])(sm_object *o0, sm_object *o1);
extern sm_object *(*sm_gt_functions[])(sm_object *o0, sm_object *o1);
extern sm_object *(*sm_lt_functions[])(sm_object *o0, sm_object *o1);

// Types: ui8 ui64 i64 f64 cx

// Add operations
sm_object *sm_add() {
  sm_object *o0 = sm_pop();
  sm_object *o1 = sm_pop();
  return sm_add_functions[o0->my_type * 4 + o1->my_type](o0, o1);
}

// uint8

sm_object *sm_add_ui8_and_ui8(sm_object *o0, sm_object *o1) {
  uint8_t a = ((sm_ui8 *)o0)->value;
  uint8_t b = ((sm_ui8 *)o1)->value;
  return (sm_object *)sm_new_ui8(a + b);
}

sm_object *sm_add_ui8_and_ui64(sm_object *o0, sm_object *o1) {
  uint8_t  a = ((sm_ui8 *)o0)->value;
  uint64_t b = ((sm_ui64 *)o1)->value;
  return (sm_object *)sm_new_ui64(a + b);
}

sm_object *sm_add_ui64_and_ui8(sm_object *o0, sm_object *o1) {
  uint64_t a = ((sm_ui64 *)o0)->value;
  uint8_t  b = ((sm_ui8 *)o1)->value;
  return (sm_object *)sm_new_ui64(a + b);
}

sm_object *sm_add_ui8_and_i64(sm_object *o0, sm_object *o1) {
  uint8_t a = ((sm_ui8 *)o0)->value;
  int64_t b = ((sm_i64 *)o1)->value;
  return (sm_object *)sm_new_i64(a + b);
}

sm_object *sm_add_i64_and_ui8(sm_object *o0, sm_object *o1) {
  int64_t a = ((sm_i64 *)o0)->value;
  uint8_t b = ((sm_ui8 *)o1)->value;
  return (sm_object *)sm_new_i64(a + b);
}

sm_object *sm_add_f64_and_ui8(sm_object *o0, sm_object *o1) {
  double  a = ((sm_f64 *)o0)->value;
  uint8_t b = ((sm_ui8 *)o1)->value;
  return (sm_object *)sm_new_f64(a + b);
}

sm_object *sm_add_ui8_and_f64(sm_object *o0, sm_object *o1) {
  uint8_t a = ((sm_ui8 *)o0)->value;
  double  b = ((sm_f64 *)o1)->value;
  return (sm_object *)sm_new_f64(a + b);
}

// ui64

sm_object *sm_add_ui64_and_ui64(sm_object *o0, sm_object *o1) {
  uint64_t a = ((sm_ui64 *)o0)->value;
  uint64_t b = ((sm_ui64 *)o1)->value;
  return (sm_object *)sm_new_ui64(a + b);
}

sm_object *sm_add_ui64_and_i64(sm_object *o0, sm_object *o1) {
  uint64_t a = ((sm_ui64 *)o0)->value;
  int64_t  b = ((sm_i64 *)o1)->value;
  return (sm_object *)sm_new_i64(a + b);
}

sm_object *sm_add_i64_and_ui64(sm_object *o0, sm_object *o1) {
  int64_t  a = ((sm_i64 *)o0)->value;
  uint64_t b = ((sm_ui64 *)o1)->value;
  return (sm_object *)sm_new_i64(a + b);
}

sm_object *sm_add_f64_and_ui64(sm_object *o0, sm_object *o1) {
  double   a = ((sm_f64 *)o0)->value;
  uint64_t b = ((sm_ui64 *)o1)->value;
  return (sm_object *)sm_new_f64(a + b);
}

sm_object *sm_add_ui64_and_f64(sm_object *o0, sm_object *o1) {
  uint64_t a = ((sm_f64 *)o0)->value;
  double   b = ((sm_ui64 *)o1)->value;
  return (sm_object *)sm_new_f64(a + b);
}

// i64

sm_object *sm_add_i64_and_i64(sm_object *o0, sm_object *o1) {
  int64_t a = ((sm_i64 *)o0)->value;
  int64_t b = ((sm_i64 *)o1)->value;
  return (sm_object *)sm_new_i64(a + b);
}

sm_object *sm_add_i64_and_f64(sm_object *o0, sm_object *o1) {
  int64_t a = ((sm_i64 *)o0)->value;
  double  b = ((sm_f64 *)o1)->value;
  return (sm_object *)sm_new_f64(a + b);
}

sm_object *sm_add_f64_and_i64(sm_object *o0, sm_object *o1) {
  double  a = ((sm_f64 *)o0)->value;
  int64_t b = ((sm_i64 *)o1)->value;
  return (sm_object *)sm_new_f64(a + b);
}

// f64

sm_object *sm_add_f64_and_f64(sm_object *o0, sm_object *o1) {
  double a = ((sm_f64 *)o0)->value;
  double b = ((sm_f64 *)o1)->value;
  return (sm_object *)sm_new_f64(a + b);
}

// cx
sm_object *sm_add_cx_and_number(sm_object *cx, sm_object *number) {
  sm_cx     *cx_number    = (sm_cx *)cx;
  sm_object *add_function = sm_cx_get_far(cx_number, sm_new_symbol("_add_", 5));
  // TODO: you have to push the stack frame and cx ,and then execute_fun
  return cx;
}


// Minus operations

sm_object *sm_minus() {
  sm_object *o0 = sm_pop();
  sm_object *o1 = sm_pop();
  return sm_minus_functions[o0->my_type * 4 + o1->my_type](o0, o1);
}

// uint8

sm_object *sm_minus_ui8_and_ui8(sm_object *o0, sm_object *o1) {
  uint8_t a = ((sm_ui8 *)o0)->value;
  uint8_t b = ((sm_ui8 *)o1)->value;
  return (sm_object *)sm_new_ui8(a - b);
}

sm_object *sm_minus_ui8_and_ui64(sm_object *o0, sm_object *o1) {
  uint8_t  a = ((sm_ui8 *)o0)->value;
  uint64_t b = ((sm_ui64 *)o1)->value;
  return (sm_object *)sm_new_ui64(a - b);
}

sm_object *sm_minus_ui64_and_ui8(sm_object *o0, sm_object *o1) {
  uint64_t a = ((sm_ui64 *)o0)->value;
  uint8_t  b = ((sm_ui8 *)o1)->value;
  return (sm_object *)sm_new_ui64(a - b);
}

sm_object *sm_minus_ui8_and_i64(sm_object *o0, sm_object *o1) {
  uint8_t a = ((sm_ui8 *)o0)->value;
  int64_t b = ((sm_i64 *)o1)->value;
  return (sm_object *)sm_new_i64(a - b);
}

sm_object *sm_minus_i64_and_ui8(sm_object *o0, sm_object *o1) {
  int64_t a = ((sm_i64 *)o0)->value;
  uint8_t b = ((sm_ui8 *)o1)->value;
  return (sm_object *)sm_new_i64(a - b);
}

sm_object *sm_minus_f64_and_ui8(sm_object *o0, sm_object *o1) {
  double  a = ((sm_f64 *)o0)->value;
  uint8_t b = ((sm_ui8 *)o1)->value;
  return (sm_object *)sm_new_f64(a - b);
}

sm_object *sm_minus_ui8_and_f64(sm_object *o0, sm_object *o1) {
  uint8_t a = ((sm_ui8 *)o0)->value;
  double  b = ((sm_f64 *)o1)->value;
  return (sm_object *)sm_new_f64(a - b);
}

// ui64

sm_object *sm_minus_ui64_and_ui64(sm_object *o0, sm_object *o1) {
  uint64_t a = ((sm_ui64 *)o0)->value;
  uint64_t b = ((sm_ui64 *)o1)->value;
  return (sm_object *)sm_new_ui64(a - b);
}

sm_object *sm_minus_ui64_and_i64(sm_object *o0, sm_object *o1) {
  uint64_t a = ((sm_ui64 *)o0)->value;
  int64_t  b = ((sm_i64 *)o1)->value;
  return (sm_object *)sm_new_i64(a - b);
}

sm_object *sm_minus_i64_and_ui64(sm_object *o0, sm_object *o1) {
  int64_t  a = ((sm_i64 *)o0)->value;
  uint64_t b = ((sm_ui64 *)o1)->value;
  return (sm_object *)sm_new_i64(a - b);
}

sm_object *sm_minus_f64_and_ui64(sm_object *o0, sm_object *o1) {
  double   a = ((sm_f64 *)o0)->value;
  uint64_t b = ((sm_ui64 *)o1)->value;
  return (sm_object *)sm_new_f64(a - b);
}

sm_object *sm_minus_ui64_and_f64(sm_object *o0, sm_object *o1) {
  uint64_t a = ((sm_f64 *)o0)->value;
  double   b = ((sm_ui64 *)o1)->value;
  return (sm_object *)sm_new_f64(a - b);
}

// i64

sm_object *sm_minus_i64_and_i64(sm_object *o0, sm_object *o1) {
  int64_t a = ((sm_i64 *)o0)->value;
  int64_t b = ((sm_i64 *)o1)->value;
  return (sm_object *)sm_new_i64(a - b);
}

sm_object *sm_minus_i64_and_f64(sm_object *o0, sm_object *o1) {
  int64_t a = ((sm_i64 *)o0)->value;
  double  b = ((sm_f64 *)o1)->value;
  return (sm_object *)sm_new_f64(a - b);
}

sm_object *sm_minus_f64_and_i64(sm_object *o0, sm_object *o1) {
  double  a = ((sm_f64 *)o0)->value;
  int64_t b = ((sm_i64 *)o1)->value;
  return (sm_object *)sm_new_f64(a - b);
}

// f64

sm_object *sm_minus_f64_and_f64(sm_object *o0, sm_object *o1) {
  double a = ((sm_f64 *)o0)->value;
  double b = ((sm_f64 *)o1)->value;
  return (sm_object *)sm_new_f64(a - b);
}


// Times operations
sm_object *sm_times() {
  sm_object *o0 = sm_pop();
  sm_object *o1 = sm_pop();
  return sm_times_functions[o0->my_type * 4 + o1->my_type](o0, o1);
}

// uint8

sm_object *sm_times_ui8_and_ui8(sm_object *o0, sm_object *o1) {
  uint8_t a = ((sm_ui8 *)o0)->value;
  uint8_t b = ((sm_ui8 *)o1)->value;
  return (sm_object *)sm_new_ui8(a * b);
}

sm_object *sm_times_ui8_and_ui64(sm_object *o0, sm_object *o1) {
  uint8_t  a = ((sm_ui8 *)o0)->value;
  uint64_t b = ((sm_ui64 *)o1)->value;
  return (sm_object *)sm_new_ui64(a * b);
}

sm_object *sm_times_ui64_and_ui8(sm_object *o0, sm_object *o1) {
  uint64_t a = ((sm_ui64 *)o0)->value;
  uint8_t  b = ((sm_ui8 *)o1)->value;
  return (sm_object *)sm_new_ui64(a * b);
}

sm_object *sm_times_ui8_and_i64(sm_object *o0, sm_object *o1) {
  uint8_t a = ((sm_ui8 *)o0)->value;
  int64_t b = ((sm_i64 *)o1)->value;
  return (sm_object *)sm_new_i64(a * b);
}

sm_object *sm_times_i64_and_ui8(sm_object *o0, sm_object *o1) {
  int64_t a = ((sm_i64 *)o0)->value;
  uint8_t b = ((sm_ui8 *)o1)->value;
  return (sm_object *)sm_new_i64(a * b);
}

sm_object *sm_times_f64_and_ui8(sm_object *o0, sm_object *o1) {
  double  a = ((sm_f64 *)o0)->value;
  uint8_t b = ((sm_ui8 *)o1)->value;
  return (sm_object *)sm_new_f64(a * b);
}

sm_object *sm_times_ui8_and_f64(sm_object *o0, sm_object *o1) {
  uint8_t a = ((sm_ui8 *)o0)->value;
  double  b = ((sm_f64 *)o1)->value;
  return (sm_object *)sm_new_f64(a * b);
}

// ui64

sm_object *sm_times_ui64_and_ui64(sm_object *o0, sm_object *o1) {
  uint64_t a = ((sm_ui64 *)o0)->value;
  uint64_t b = ((sm_ui64 *)o1)->value;
  return (sm_object *)sm_new_ui64(a * b);
}

sm_object *sm_times_ui64_and_i64(sm_object *o0, sm_object *o1) {
  uint64_t a = ((sm_ui64 *)o0)->value;
  int64_t  b = ((sm_i64 *)o1)->value;
  return (sm_object *)sm_new_i64(a * b);
}

sm_object *sm_times_i64_and_ui64(sm_object *o0, sm_object *o1) {
  int64_t  a = ((sm_i64 *)o0)->value;
  uint64_t b = ((sm_ui64 *)o1)->value;
  return (sm_object *)sm_new_i64(a * b);
}

sm_object *sm_times_f64_and_ui64(sm_object *o0, sm_object *o1) {
  double   a = ((sm_f64 *)o0)->value;
  uint64_t b = ((sm_ui64 *)o1)->value;
  return (sm_object *)sm_new_f64(a * b);
}

sm_object *sm_times_ui64_and_f64(sm_object *o0, sm_object *o1) {
  uint64_t a = ((sm_ui64 *)o0)->value;
  double   b = ((sm_f64 *)o1)->value;
  return (sm_object *)sm_new_f64(a * b);
}

// i64

sm_object *sm_times_i64_and_i64(sm_object *o0, sm_object *o1) {
  int64_t a = ((sm_i64 *)o0)->value;
  int64_t b = ((sm_i64 *)o1)->value;
  return (sm_object *)sm_new_i64(a * b);
}

sm_object *sm_times_i64_and_f64(sm_object *o0, sm_object *o1) {
  int64_t a = ((sm_i64 *)o0)->value;
  double  b = ((sm_f64 *)o1)->value;
  return (sm_object *)sm_new_f64(a * b);
}

sm_object *sm_times_f64_and_i64(sm_object *o0, sm_object *o1) {
  double  a = ((sm_f64 *)o0)->value;
  int64_t b = ((sm_i64 *)o1)->value;
  return (sm_object *)sm_new_f64(a * b);
}

// f64

sm_object *sm_times_f64_and_f64(sm_object *o0, sm_object *o1) {
  double a = ((sm_f64 *)o0)->value;
  double b = ((sm_f64 *)o1)->value;
  return (sm_object *)sm_new_f64(a * b);
}


// Divide operations
sm_object *sm_divide() {
  sm_object *o0 = sm_pop();
  sm_object *o1 = sm_pop();
  return sm_divide_functions[o0->my_type * 4 + o1->my_type](o0, o1);
}
// uint8

sm_object *sm_divide_ui8_and_ui8(sm_object *o0, sm_object *o1) {
  uint8_t a = ((sm_ui8 *)o0)->value;
  uint8_t b = ((sm_ui8 *)o1)->value;
  return (sm_object *)sm_new_ui8(a / b);
}

sm_object *sm_divide_ui8_and_ui64(sm_object *o0, sm_object *o1) {
  uint8_t  a = ((sm_ui8 *)o0)->value;
  uint64_t b = ((sm_ui64 *)o1)->value;
  return (sm_object *)sm_new_ui64(a / b);
}

sm_object *sm_divide_ui64_and_ui8(sm_object *o0, sm_object *o1) {
  uint64_t a = ((sm_ui64 *)o0)->value;
  uint8_t  b = ((sm_ui8 *)o1)->value;
  return (sm_object *)sm_new_ui64(a / b);
}

sm_object *sm_divide_ui8_and_i64(sm_object *o0, sm_object *o1) {
  uint8_t a = ((sm_ui8 *)o0)->value;
  int64_t b = ((sm_i64 *)o1)->value;
  return (sm_object *)sm_new_i64(a / b);
}

sm_object *sm_divide_i64_and_ui8(sm_object *o0, sm_object *o1) {
  int64_t a = ((sm_i64 *)o0)->value;
  uint8_t b = ((sm_ui8 *)o1)->value;
  return (sm_object *)sm_new_i64(a / b);
}

sm_object *sm_divide_f64_and_ui8(sm_object *o0, sm_object *o1) {
  double  a = ((sm_f64 *)o0)->value;
  uint8_t b = ((sm_ui8 *)o1)->value;
  return (sm_object *)sm_new_f64(a / b);
}

sm_object *sm_divide_ui8_and_f64(sm_object *o0, sm_object *o1) {
  uint8_t a = ((sm_ui8 *)o0)->value;
  double  b = ((sm_f64 *)o1)->value;
  return (sm_object *)sm_new_f64(a / b);
}

// ui64

sm_object *sm_divide_ui64_and_ui64(sm_object *o0, sm_object *o1) {
  uint64_t a = ((sm_ui64 *)o0)->value;
  uint64_t b = ((sm_ui64 *)o1)->value;
  return (sm_object *)sm_new_ui64(a / b);
}

sm_object *sm_divide_ui64_and_i64(sm_object *o0, sm_object *o1) {
  uint64_t a = ((sm_ui64 *)o0)->value;
  int64_t  b = ((sm_i64 *)o1)->value;
  return (sm_object *)sm_new_i64(a / b);
}

sm_object *sm_divide_i64_and_ui64(sm_object *o0, sm_object *o1) {
  int64_t  a = ((sm_i64 *)o0)->value;
  uint64_t b = ((sm_ui64 *)o1)->value;
  return (sm_object *)sm_new_i64(a / b);
}

sm_object *sm_divide_f64_and_ui64(sm_object *o0, sm_object *o1) {
  double   a = ((sm_f64 *)o0)->value;
  uint64_t b = ((sm_ui64 *)o1)->value;
  return (sm_object *)sm_new_f64(a / b);
}

sm_object *sm_divide_ui64_and_f64(sm_object *o0, sm_object *o1) {
  uint64_t a = ((sm_f64 *)o0)->value;
  double   b = ((sm_ui64 *)o1)->value;
  return (sm_object *)sm_new_f64(a / b);
}

// i64

sm_object *sm_divide_i64_and_i64(sm_object *o0, sm_object *o1) {
  int64_t a = ((sm_i64 *)o0)->value;
  int64_t b = ((sm_i64 *)o1)->value;
  return (sm_object *)sm_new_i64(a / b);
}

sm_object *sm_divide_i64_and_f64(sm_object *o0, sm_object *o1) {
  int64_t a = ((sm_i64 *)o0)->value;
  double  b = ((sm_f64 *)o1)->value;
  return (sm_object *)sm_new_f64(a / b);
}

sm_object *sm_divide_f64_and_i64(sm_object *o0, sm_object *o1) {
  double  a = ((sm_f64 *)o0)->value;
  int64_t b = ((sm_i64 *)o1)->value;
  return (sm_object *)sm_new_f64(a / b);
}

// f64

sm_object *sm_divide_f64_and_f64(sm_object *o0, sm_object *o1) {
  double a = ((sm_f64 *)o0)->value;
  double b = ((sm_f64 *)o1)->value;
  return (sm_object *)sm_new_f64(a / b);
}

// cx
sm_object *sm_divide_cx_and_number(sm_object *cx, sm_object *number) {
  sm_cx     *cx_number    = (sm_cx *)cx;
  sm_object *add_function = sm_cx_get_far(cx_number, sm_new_symbol("_divide_", 5));
  // TODO: you have to push the stack frame and cx ,and then execute_fun
  return cx;
}


// GT operations
sm_object *sm_gt() {
  sm_object *o0 = sm_pop();
  sm_object *o1 = sm_pop();
  return sm_gt_functions[o0->my_type * 4 + o1->my_type](o0, o1);
}

// uint8

sm_object *sm_gt_ui8_and_ui8(sm_object *o0, sm_object *o1) {
  uint8_t a = ((sm_ui8 *)o0)->value;
  uint8_t b = ((sm_ui8 *)o1)->value;
  if (a > b)
    return (sm_object *)sms_true;
  return (sm_object *)sms_false;
}

sm_object *sm_gt_ui8_and_ui64(sm_object *o0, sm_object *o1) {
  uint8_t  a = ((sm_ui8 *)o0)->value;
  uint64_t b = ((sm_ui64 *)o1)->value;
  if (a > b)
    return (sm_object *)sms_true;
  return (sm_object *)sms_false;
}

sm_object *sm_gt_ui64_and_ui8(sm_object *o0, sm_object *o1) {
  uint64_t a = ((sm_ui64 *)o0)->value;
  uint8_t  b = ((sm_ui8 *)o1)->value;
  if (a > b)
    return (sm_object *)sms_true;
  return (sm_object *)sms_false;
}

sm_object *sm_gt_ui8_and_i64(sm_object *o0, sm_object *o1) {
  uint8_t a = ((sm_ui8 *)o0)->value;
  int64_t b = ((sm_i64 *)o1)->value;
  if (a > b)
    return (sm_object *)sms_true;
  return (sm_object *)sms_false;
}

sm_object *sm_gt_i64_and_ui8(sm_object *o0, sm_object *o1) {
  int64_t a = ((sm_i64 *)o0)->value;
  uint8_t b = ((sm_ui8 *)o1)->value;
  if (a > b)
    return (sm_object *)sms_true;
  return (sm_object *)sms_false;
}

sm_object *sm_gt_f64_and_ui8(sm_object *o0, sm_object *o1) {
  double  a = ((sm_f64 *)o0)->value;
  uint8_t b = ((sm_ui8 *)o1)->value;
  if (a > b)
    return (sm_object *)sms_true;
  return (sm_object *)sms_false;
}

sm_object *sm_gt_ui8_and_f64(sm_object *o0, sm_object *o1) {
  uint8_t a = ((sm_ui8 *)o0)->value;
  double  b = ((sm_f64 *)o1)->value;
  if (a > b)
    return (sm_object *)sms_true;
  return (sm_object *)sms_false;
}

// ui64

sm_object *sm_gt_ui64_and_ui64(sm_object *o0, sm_object *o1) {
  uint64_t a = ((sm_ui64 *)o0)->value;
  uint64_t b = ((sm_ui64 *)o1)->value;
  if (a > b)
    return (sm_object *)sms_true;
  return (sm_object *)sms_false;
}

sm_object *sm_gt_ui64_and_i64(sm_object *o0, sm_object *o1) {
  uint64_t a = ((sm_ui64 *)o0)->value;
  int64_t  b = ((sm_i64 *)o1)->value;
  if (a > b)
    return (sm_object *)sms_true;
  return (sm_object *)sms_false;
}

sm_object *sm_gt_i64_and_ui64(sm_object *o0, sm_object *o1) {
  int64_t  a = ((sm_i64 *)o0)->value;
  uint64_t b = ((sm_ui64 *)o1)->value;
  if (a > b)
    return (sm_object *)sms_true;
  return (sm_object *)sms_false;
}

sm_object *sm_gt_f64_and_ui64(sm_object *o0, sm_object *o1) {
  double   a = ((sm_f64 *)o0)->value;
  uint64_t b = ((sm_ui64 *)o1)->value;
  if (a > b)
    return (sm_object *)sms_true;
  return (sm_object *)sms_false;
}

sm_object *sm_gt_ui64_and_f64(sm_object *o0, sm_object *o1) {
  uint64_t a = ((sm_f64 *)o0)->value;
  double   b = ((sm_ui64 *)o1)->value;
  if (a > b)
    return (sm_object *)sms_true;
  return (sm_object *)sms_false;
}

// i64

sm_object *sm_gt_i64_and_i64(sm_object *o0, sm_object *o1) {
  int64_t a = ((sm_i64 *)o0)->value;
  int64_t b = ((sm_i64 *)o1)->value;
  if (a > b)
    return (sm_object *)sms_true;
  return (sm_object *)sms_false;
}

sm_object *sm_gt_i64_and_f64(sm_object *o0, sm_object *o1) {
  int64_t a = ((sm_i64 *)o0)->value;
  double  b = ((sm_f64 *)o1)->value;
  if (a > b)
    return (sm_object *)sms_true;
  return (sm_object *)sms_false;
}

sm_object *sm_gt_f64_and_i64(sm_object *o0, sm_object *o1) {
  double  a = ((sm_f64 *)o0)->value;
  int64_t b = ((sm_i64 *)o1)->value;
  if (a > b)
    return (sm_object *)sms_true;
  return (sm_object *)sms_false;
}

// f64

sm_object *sm_gt_f64_and_f64(sm_object *o0, sm_object *o1) {
  double a = ((sm_f64 *)o0)->value;
  double b = ((sm_f64 *)o1)->value;
  if (a > b)
    return (sm_object *)sms_true;
  return (sm_object *)sms_false;
}

// cx
sm_object *sm_gt_cx_and_number(sm_object *cx, sm_object *number) {
  sm_cx     *cx_number    = (sm_cx *)cx;
  sm_object *add_function = sm_cx_get_far(cx_number, sm_new_symbol("_gt_", 5));
  // TODO: you have to push the stack frame and cx ,and then execute_fun
  return cx;
}


// lt operations
sm_object *sm_lt() {
  sm_object *o0 = sm_pop();
  sm_object *o1 = sm_pop();
  return sm_lt_functions[o0->my_type * 4 + o1->my_type](o0, o1);
}

// uint8

sm_object *sm_lt_ui8_and_ui8(sm_object *o0, sm_object *o1) {
  uint8_t a = ((sm_ui8 *)o0)->value;
  uint8_t b = ((sm_ui8 *)o1)->value;
  if (a < b)
    return (sm_object *)sms_true;
  return (sm_object *)sms_false;
}

sm_object *sm_lt_ui8_and_ui64(sm_object *o0, sm_object *o1) {
  uint8_t  a = ((sm_ui8 *)o0)->value;
  uint64_t b = ((sm_ui64 *)o1)->value;
  if (a < b)
    return (sm_object *)sms_true;
  return (sm_object *)sms_false;
}

sm_object *sm_lt_ui64_and_ui8(sm_object *o0, sm_object *o1) {
  uint64_t a = ((sm_ui64 *)o0)->value;
  uint8_t  b = ((sm_ui8 *)o1)->value;
  if (a < b)
    return (sm_object *)sms_true;
  return (sm_object *)sms_false;
}

sm_object *sm_lt_ui8_and_i64(sm_object *o0, sm_object *o1) {
  uint8_t a = ((sm_ui8 *)o0)->value;
  int64_t b = ((sm_i64 *)o1)->value;
  if (a < b)
    return (sm_object *)sms_true;
  return (sm_object *)sms_false;
}

sm_object *sm_lt_i64_and_ui8(sm_object *o0, sm_object *o1) {
  int64_t a = ((sm_i64 *)o0)->value;
  uint8_t b = ((sm_ui8 *)o1)->value;
  if (a < b)
    return (sm_object *)sms_true;
  return (sm_object *)sms_false;
}

sm_object *sm_lt_f64_and_ui8(sm_object *o0, sm_object *o1) {
  double  a = ((sm_f64 *)o0)->value;
  uint8_t b = ((sm_ui8 *)o1)->value;
  if (a < b)
    return (sm_object *)sms_true;
  return (sm_object *)sms_false;
}

sm_object *sm_lt_ui8_and_f64(sm_object *o0, sm_object *o1) {
  uint8_t a = ((sm_ui8 *)o0)->value;
  double  b = ((sm_f64 *)o1)->value;
  if (a < b)
    return (sm_object *)sms_true;
  return (sm_object *)sms_false;
}

// ui64

sm_object *sm_lt_ui64_and_ui64(sm_object *o0, sm_object *o1) {
  uint64_t a = ((sm_ui64 *)o0)->value;
  uint64_t b = ((sm_ui64 *)o1)->value;
  if (a < b)
    return (sm_object *)sms_true;
  return (sm_object *)sms_false;
}

sm_object *sm_lt_ui64_and_i64(sm_object *o0, sm_object *o1) {
  uint64_t a = ((sm_ui64 *)o0)->value;
  int64_t  b = ((sm_i64 *)o1)->value;
  if (a < b)
    return (sm_object *)sms_true;
  return (sm_object *)sms_false;
}

sm_object *sm_lt_i64_and_ui64(sm_object *o0, sm_object *o1) {
  int64_t  a = ((sm_i64 *)o0)->value;
  uint64_t b = ((sm_ui64 *)o1)->value;
  if (a < b)
    return (sm_object *)sms_true;
  return (sm_object *)sms_false;
}

sm_object *sm_lt_f64_and_ui64(sm_object *o0, sm_object *o1) {
  double   a = ((sm_f64 *)o0)->value;
  uint64_t b = ((sm_ui64 *)o1)->value;
  if (a < b)
    return (sm_object *)sms_true;
  return (sm_object *)sms_false;
}

sm_object *sm_lt_ui64_and_f64(sm_object *o0, sm_object *o1) {
  uint64_t a = ((sm_f64 *)o0)->value;
  double   b = ((sm_ui64 *)o1)->value;
  if (a < b)
    return (sm_object *)sms_true;
  return (sm_object *)sms_false;
}

// i64

sm_object *sm_lt_i64_and_i64(sm_object *o0, sm_object *o1) {
  int64_t a = ((sm_i64 *)o0)->value;
  int64_t b = ((sm_i64 *)o1)->value;
  if (a < b)
    return (sm_object *)sms_true;
  return (sm_object *)sms_false;
}

sm_object *sm_lt_i64_and_f64(sm_object *o0, sm_object *o1) {
  int64_t a = ((sm_i64 *)o0)->value;
  double  b = ((sm_f64 *)o1)->value;
  if (a < b)
    return (sm_object *)sms_true;
  return (sm_object *)sms_false;
}

sm_object *sm_lt_f64_and_i64(sm_object *o0, sm_object *o1) {
  double  a = ((sm_f64 *)o0)->value;
  int64_t b = ((sm_i64 *)o1)->value;
  if (a < b)
    return (sm_object *)sms_true;
  return (sm_object *)sms_false;
}

// f64

sm_object *sm_lt_f64_and_f64(sm_object *o0, sm_object *o1) {
  double a = ((sm_f64 *)o0)->value;
  double b = ((sm_f64 *)o1)->value;
  if (a < b)
    return (sm_object *)sms_true;
  return (sm_object *)sms_false;
}

// cx
sm_object *sm_lt_cx_and_number(sm_object *cx, sm_object *number) {
  sm_cx     *cx_number    = (sm_cx *)cx;
  sm_object *add_function = sm_cx_get_far(cx_number, sm_new_symbol("_lt_", 5));
  // TODO: you have to push the stack frame and cx ,and then execute_fun
  return cx;
}
