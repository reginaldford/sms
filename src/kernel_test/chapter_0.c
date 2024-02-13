#include "../main/sms.h"
#include "macros.h"

// Just report the size of structures
int32_t chapter_0(int32_t test) {
  int32_t current_test = 0;
  TEST(printf("sizeof %s is %d\n", "enum", (int32_t)sizeof(enum sm_object_type)));
  TEST(printf("sizeof %s is %d\n", "long", (int32_t)sizeof(long)));
  TEST(printf("sizeof %s is %d\n", "long long", (int32_t)sizeof(long long)));
  TEST(printf("sizeof %s is %d\n", "double", (int32_t)sizeof(double)));
  TEST(printf("sizeof %s is %d\n", "int32_t", (int32_t)sizeof(int32_t)));
  TEST(printf("sizeof %s is %d\n", "short", (int32_t)sizeof(int32_t)));
  TEST(printf("sizeof %s is %d\n", "sm_object*", (int32_t)sizeof(sm_object *)));
  TEST(printf("sizeof %s is %d\n", "sm_object", (int32_t)sizeof(sm_object)));
  TEST(printf("sizeof %s is %d\n", "sm_string", (int32_t)sizeof(sm_string)));
  TEST(printf("sizeof %s is %d\n", "sm_cx", (int32_t)sizeof(sm_cx)));
  TEST(printf("sizeof %s is %d\n", "sm_double", (int32_t)sizeof(sm_double)));
  TEST(printf("sizeof %s is %d\n", "sm_pointer", (int32_t)sizeof(sm_pointer)));
  TEST(printf("sizeof %s is %d\n", "sm_symbol", (int32_t)sizeof(sm_symbol)));
  TEST(printf("sizeof %s is %d\n", "sm_expr", (int32_t)sizeof(sm_expr)));
  TEST(printf("sizeof %s is %d\n", "sm_fun", (int32_t)sizeof(sm_fun)));
  TEST(printf("sizeof %s is %d\n", "sm_fun_param", (int32_t)sizeof(sm_fun_param)));
  TEST(printf("sizeof %s is %d\n", "sm_fun_param_obj", (int32_t)sizeof(sm_fun_param_obj)));
  TEST(printf("sizeof %s is %d\n", "sm_error", (int32_t)sizeof(sm_error)));
  TEST(printf("sizeof %s is %d\n", "sm_space", (int32_t)sizeof(sm_space)));
  TEST(printf("sizeof %s is %d\n", "sm_return", (int32_t)sizeof(sm_return)));
  TEST(printf("sizeof %s is %d\n", "sm_self", (int32_t)sizeof(sm_self)));
  return 0;
}
