// This project is licensed under the BSD 2 clause license. See LICENSE.txt for more information.
#include "../sms.h"
int main() {
  printf("sizeof %s is %d\n", "long", (int)sizeof(long));
  printf("sizeof %s is %d\n", "int", (int)sizeof(int));
  printf("sizeof %s is %d\n", "short", (int)sizeof(int));
  printf("sizeof %s is %d\n", "short int", (int)sizeof(short int));
  printf("sizeof %s is %d\n", "sm_object*", (int)sizeof(sm_object *));
  printf("sizeof %s is %d\n", "sm_object", (int)sizeof(sm_object));
  printf("sizeof %s is %d\n", "sm_string", (int)sizeof(sm_string));
  printf("sizeof %s is %d\n", "sm_context", (int)sizeof(sm_context));
  printf("sizeof %s is %d\n", "sm_double", (int)sizeof(sm_double));
  printf("sizeof %s is %d\n", "sm_pointer", (int)sizeof(sm_pointer));
  printf("sizeof %s is %d\n", "sm_symbol", (int)sizeof(sm_symbol));
  printf("sizeof %s is %d\n", "sm_context_entry", (int)sizeof(sm_context_entry));
  printf("sizeof %s is %d\n", "sm_key_value", (int)sizeof(sm_key_value));
  printf("sizeof %s is %d\n", "sm_expression", (int)sizeof(sm_expression));
  printf("sizeof %s is %d\n", "sm_spacer", (int)sizeof(sm_spacer));
}
