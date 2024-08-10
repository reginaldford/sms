#include "../main/sms.h"
#include "macros.h"

// Just report the size of structures
int chapter_0(int test) {
  int current_test = 0;
  TEST(printf("sizeof %s is %d\n", "enum", (int)sizeof(enum sm_object_type)));
  TEST(printf("sizeof %s is %d\n", "long", (int)sizeof(long)));
  TEST(printf("sizeof %s is %d\n", "long long", (int)sizeof(long long)));
  TEST(printf("sizeof %s is %d\n", "f64", (int)sizeof(f64)));
  TEST(printf("sizeof %s is %d\n", "int", (int)sizeof(int)));
  TEST(printf("sizeof %s is %d\n", "short", (int)sizeof(int)));
  TEST(printf("sizeof %s is %d\n", "sm_object*", (int)sizeof(sm_object *)));
  TEST(printf("sizeof %s is %d\n", "sm_object", (int)sizeof(sm_object)));
  TEST(printf("sizeof %s is %d\n", "sm_string", (int)sizeof(sm_string)));
  TEST(printf("sizeof %s is %d\n", "sm_cx", (int)sizeof(sm_cx)));
  TEST(printf("sizeof %s is %d\n", "sm_f64", (int)sizeof(sm_f64)));
  TEST(printf("sizeof %s is %d\n", "sm_pointer", (int)sizeof(sm_pointer)));
  TEST(printf("sizeof %s is %d\n", "sm_symbol", (int)sizeof(sm_symbol)));
  TEST(printf("sizeof %s is %d\n", "sm_expr", (int)sizeof(sm_expr)));
  TEST(printf("sizeof %s is %d\n", "sm_fun", (int)sizeof(sm_fun)));
  TEST(printf("sizeof %s is %d\n", "sm_fun_param", (int)sizeof(sm_fun_param)));
  TEST(printf("sizeof %s is %d\n", "sm_fun_param_obj", (int)sizeof(sm_fun_param_obj)));
  TEST(printf("sizeof %s is %d\n", "sm_error", (int)sizeof(sm_error)));
  TEST(printf("sizeof %s is %d\n", "sm_space", (int)sizeof(sm_space)));
  TEST(printf("sizeof %s is %d\n", "sm_return", (int)sizeof(sm_return)));
  TEST(printf("sizeof %s is %d\n", "sm_self", (int)sizeof(sm_self)));
  return 0;
}
