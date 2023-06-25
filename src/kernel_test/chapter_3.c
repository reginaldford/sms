#include "../main/sms.h"
#include "macros.h"

// This chapter checks that contexts can scope and that memory is managed correctly
int chapter_3(int test) {
  int num_fails = 0;
  if (test != -1)
    printf("This chapter does not take a test number.\n");

  sm_env env;
  env.mem_flag = false;
  sm_init(&env, 0, NULL);

  sm_cx *test_parent_cx = sm_new_cx(NULL);
  sm_cx *test_cx        = sm_new_cx(test_parent_cx);

  sm_cx_let(test_parent_cx, "a", 1, (sm_object *)sm_new_double(1));
  sm_cx_let(test_parent_cx, "b", 1, (sm_object *)sm_new_double(2));
  sm_cx_let(test_parent_cx, "ab", 2, (sm_object *)sm_new_double(3));

  sm_cx_let(test_cx, "a", 1, (sm_object *)sm_new_double(7));
  sm_cx_let(test_cx, "b", 1, (sm_object *)sm_new_double(8));
  sm_cx_let(test_cx, "ab", 2, (sm_object *)sm_new_double(9));

  sm_object *a = sm_cx_get(test_cx, "a", 1);

  sm_garbage_collect();

  sm_cx_rm(test_cx, "b", 1);

  return num_fails;
}
