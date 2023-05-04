#include "../main/sms.h"
#include "macros.h"

// Since the sms_test system only checks that to_string(evaluated)==to_string(literal)
// Proving that literals print to what they should to verify expression printing mechanics.
int chapter_3(int test) {
  int num_fails = 0;
  sm_init(NULL);

  sm_cx *test_parent_cx = sm_new_cx(NULL);
  sm_cx *test_cx        = sm_new_cx(test_parent_cx);
  sm_cx_let(test_parent_cx, "PI", 2, (sm_object *)sm_new_double(3.14));
  sm_cx_let(test_parent_cx, "PI", 2, (sm_object *)sm_new_double(3.14));
  sm_cx_let(test_parent_cx, "PI", 2, (sm_object *)sm_new_double(3.14));
  sm_cx_let(test_cx, "a", 1, (sm_object *)sm_new_double(1));
  // sm_cx_let(test_cx,"b",1,(sm_object*)sm_new_double(2));

  sm_global_cx(test_cx);
  sm_garbage_collect();
  // test_cx = printf("%s\n", &sm_object_to_string((sm_object *)test_cx)->content);
  // sm_cx_rm(test_cx,"b",1);

  return num_fails;
}
