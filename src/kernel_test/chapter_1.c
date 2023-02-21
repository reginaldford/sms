#include "../main/sms.h"
#include "macros.h"

// Generating many objects and then collecting the garbage.
int chapter_1(int test) {
  sm_init();
  for (int i = 0; i < 100; i++)
    sm_new_context(0, 0, NULL);
  sm_garbage_collect();
  return 0;
}
