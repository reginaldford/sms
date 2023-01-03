#include "../sms.h"
#include <stdio.h>
int main() {
  printf("sizeof %s is %d, effectively %d\n", "sm_object", sizeof(sm_object),
         sm_round_size(sizeof(sm_object)));
  printf("sizeof %s is %d, effectively %d\n", "sm_pointer", sizeof(sm_pointer),
         sm_round_size(sizeof(sm_pointer)));
  printf("sizeof %s is %d, effectively %d\n", "sm_string", sizeof(sm_string),
         sm_round_size(sizeof(sm_string)));
  printf("sizeof %s is %d, effectively %d\n", "sm_array", sizeof(sm_array),
         sm_round_size(sizeof(sm_array)));
  printf("sizeof %s is %d, effectively %d\n", "sm_context", sizeof(sm_context),
         sm_round_size(sizeof(sm_context)));
  printf("sizeof %s is %d, effectively %d\n", "sm_double", sizeof(sm_double),
         sm_round_size(sizeof(sm_double)));
  printf("sizeof %s is %d, effectively %d\n", "long", sizeof(long), sm_round_size(sizeof(long)));
  printf("sizeof %s is %d, effectively %d\n", "int", sizeof(int), sm_round_size(sizeof(int)));
  printf("sizeof %s is %d, effectively %d\n", "sm_symbol", sizeof(sm_symbol),
         sm_round_size(sizeof(sm_symbol)));
  printf("sizeof %s is %d, effectively %d\n", "sm_context_entry", sizeof(sm_context_entry),
         sm_round_size(sizeof(sm_context_entry)));

  printf("sizeof %s is %d, effectively %d\n", "sm_key_value", sizeof(sm_key_value),
         sm_round_size(sizeof(sm_key_value)));

}
