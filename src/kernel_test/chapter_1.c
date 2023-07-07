#include "../main/sms.h"
#include "macros.h"

// Generating many objects and then collecting the garbage.
int chapter_1(int test) {
  int           num_fails = 0;
  static sm_env env;
  env.mem_flag = false;
  sm_init(&env, 0, NULL, true);
  printf("Generating 100 empty contexts.\n");
  for (int i = 0; i < 100; i++)
    sm_new_cx(NULL);
  printf("Running GC.\n");
  sm_garbage_collect();
  sm_string      *s  = sm_new_string(0, "");
  sm_parse_result pr = sm_parse_cstr(&(s->content), 0);
  printf("Evaluating empty string.\n");
  // We do not ask engine_eval to handle NULL because it expects obj->my_type to exist
  // sm_object * obj = sm_engine_eval(pr.parsed_object,NULL,NULL);
  if (pr.return_val == 0 && pr.parsed_object == NULL)
    printf("Parsed empty string, and got {0,NULL} as expected.\n");
  else {
    printf("Did not get {0,NULL} from parsing an empty string!\n");
    num_fails++;
  }
  return 0;
}
