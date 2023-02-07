#include "../sms.h"
#include "sm_test_context.h"
#include "sm_test_expr.h"
#include "sm_test_gc.h"
#include "sm_test_mathops.h"
#include "sm_test_numbers.h"
#include "sm_test_parser.h"
#include "sm_test_string.h"

const int num_tests = 8;

int perform_test(int which) {
  switch (which) {
  // test the creation of objects from the parser
  case 0:
    return sm_test_parser();
  // test the creation of objects from the parser
  case 1:
    return sm_test_numbers();
  // test context functions
  case 2:
    return sm_test_context();
  // test string functions
  case 3:
    return sm_test_string();
  // test expr functions
  case 4:
    return sm_test_expr();
  // test math functions
  case 5:
    return sm_test_mathops();
  // test garbage collector
  case 6:
    return sm_test_gc();
  case 7:
    return sm_test_numbers();
  default:
    printf("Test number: %i out of range.\n", which);
    printf("Valid test numbers are from 0 to %i.\n", num_tests - 1);
  }
  return -1;
}

int main(int num_args, char **argv) {
  int selected_test = -1;
  if (num_args > 1) {
    selected_test = atoi(argv[1]);
  }
  if (selected_test == -1) {
    int return_sum = 0;
    for (int i = 0; i < num_tests; i++) {
      return_sum += perform_test(i);
    }
    return return_sum;
  } else {
    return perform_test(selected_test);
  }
}
