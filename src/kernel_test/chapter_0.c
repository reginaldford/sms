#include "../main/sms.h"
#include "macros.h"

// Just report the size of structures
int chapter_0(int test) {
  int current_test = 0;
  // C types
  TEST(printf("sizeof %s is %zu\n", "int", sizeof(int)));
  TEST(printf("sizeof %s is %zu\n", "long long", sizeof(long long)));
  TEST(printf("sizeof %s is %zu\n", "long", sizeof(long)));
  TEST(printf("sizeof %s is %zu\n", "short", sizeof(short)));
  TEST(printf("sizeof %s is %zu\n", "size_t", sizeof(size_t)));
  // SMS types
  TEST(printf("sizeof %s is %zu\n", "sm_array", sizeof(sm_array)));
  TEST(printf("sizeof %s is %zu\n", "sm_bc_block", sizeof(sm_bc_block)));
  TEST(printf("sizeof %s is %zu\n", "sm_bstack", sizeof(sm_bstack)));
  TEST(printf("sizeof %s is %zu\n", "sm_cx", sizeof(sm_cx)));
  TEST(printf("sizeof %s is %zu\n", "sm_env", sizeof(sm_env)));
  TEST(printf("sizeof %s is %zu\n", "sm_error", sizeof(sm_error)));
  TEST(printf("sizeof %s is %zu\n", "sm_expr", sizeof(sm_expr)));
  TEST(printf("sizeof %s is %zu\n", "sm_f64", sizeof(sm_f64)));
  TEST(printf("sizeof %s is %zu\n", "sm_fun", sizeof(sm_fun)));
  TEST(printf("sizeof %s is %zu\n", "sm_fun_param", sizeof(sm_fun_param)));
  TEST(printf("sizeof %s is %zu\n", "sm_fun_param_obj", sizeof(sm_fun_param_obj)));
  TEST(printf("sizeof %s is %zu\n", "sm_heap", sizeof(sm_heap)));
  TEST(printf("sizeof %s is %zu\n", "sm_heap_set", sizeof(sm_heap_set)));
  TEST(printf("sizeof %s is %zu\n", "sm_img", sizeof(sm_img)));
  TEST(printf("sizeof %s is %zu\n", "sm_local", sizeof(sm_local)));
  TEST(printf("sizeof %s is %zu\n", "sm_meta", sizeof(sm_meta)));
  TEST(printf("sizeof %s is %zu\n", "sm_node", sizeof(sm_node)));
  TEST(printf("sizeof %s is %zu\n", "sm_object", sizeof(sm_object)));
  TEST(printf("sizeof %s is %zu\n", "sm_parse_result", sizeof(sm_parse_result)));
  TEST(printf("sizeof %s is %zu\n", "sm_pointer", sizeof(sm_pointer)));
  TEST(printf("sizeof %s is %zu\n", "sm_return", sizeof(sm_return)));
  TEST(printf("sizeof %s is %zu\n", "sm_self", sizeof(sm_self)));
  TEST(printf("sizeof %s is %zu\n", "sm_space", sizeof(sm_space)));
  TEST(printf("sizeof %s is %zu\n", "sm_stack", sizeof(sm_stack)));
  TEST(printf("sizeof %s is %zu\n", "sm_stack_obj", sizeof(sm_stack_obj)));
  TEST(printf("sizeof %s is %zu\n", "sm_string", sizeof(sm_string)));
  TEST(printf("sizeof %s is %zu\n", "sm_symbol", sizeof(sm_symbol)));
  TEST(printf("sizeof %s is %zu\n", "sm_ui8", sizeof(sm_ui8)));
  return 0;
}
