#include "../main/sms.h"
#include "macros.h"

// This chapter checks that contexts can scope and that memory is managed correctly
int chapter_3(int test) {
  int num_fails = 0;
  if (test != -1)
    printf("This chapter does not take a test number.\n");

  sm_env env;
  env.mem_flag   = false;
  env.quiet_mode = true;
  sm_init(&env, 0, NULL);

  sm_cx *test_parent_cx = sm_new_cx(NULL);
  sm_cx *test_cx        = sm_new_cx(test_parent_cx);

  sm_cx_let(test_parent_cx, sm_new_symbol("a", 1), (sm_object *)sm_new_f64(1));
  sm_cx_let(test_parent_cx, sm_new_symbol("b", 1), (sm_object *)sm_new_f64(2));
  sm_cx_let(test_parent_cx, sm_new_symbol("ab", 2), (sm_object *)sm_new_f64(3));

  sm_cx_let(test_cx, sm_new_symbol("a", 1), (sm_object *)sm_new_f64(7));
  sm_cx_let(test_cx, sm_new_symbol("b", 1), (sm_object *)sm_new_f64(8));
  sm_cx_let(test_cx, sm_new_symbol("ab", 2), (sm_object *)sm_new_f64(9));

  sm_object *a = sm_cx_get(test_cx, sm_new_symbol("a", 1));
  if (!sms_other_heap)
    sms_other_heap = sm_new_heap(sms_heap->capacity, true);
  sm_garbage_collect(sms_heap, sms_other_heap);
  sm_swap_heaps(&sms_heap, &sms_other_heap);

  sm_cx_rm(test_cx, sm_new_symbol("b", 1));

  return num_fails;
}
