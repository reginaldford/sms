#include "../main/sms.h"
#include "macros.h"

void test0() {
  static sm_env env;
  env.mem_bytes  = 1000; // only 1k bytes
  env.mem_flag   = true;
  env.quiet_mode = true;
  sm_init(&env, 0, NULL);
  printf("Constructing and deallocating 64 objects, 1000 times\n");
  fflush(stdout);
  for (int i = 0; i < 1000; i++) {
    for (int j = 0; j < 64; j++) {
      // If spaces work correctly, new function takes the space of last one
      sm_fun *junk = sm_new_fun(NULL, 0, (sm_object *)sm_new_cx(NULL));
    }
  }
  printf("We survived.\n");
}

// Testing space tuples
int chapter_4(int test) {
  int current_test = 0;
  TEST(test0());
  return 0;
}
