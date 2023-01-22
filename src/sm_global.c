// The following file is provided under the BSD 2-clause license. For more info, read LICENSE.txt.

#include "sms.h"

/* GLOBALS
Globals in this project use functions f(x):
- If x is NULL, the static field is returned.
- If x is not NULL, the old field value is returned and x replaces the field.
This allows us to use a breakpoint or printf to detect reads/writes to the global.
*/

// the global context
sm_context *sm_global_context(sm_context *replacement) {
  static sm_context *global_context = NULL;
  if (replacement != NULL) {
    sm_context *temp = global_context;
    global_context   = replacement;
    return temp;
  }
  return global_context;
}

// Current heap. Objects here are live
sm_heap *sm_global_current_heap(sm_heap *replacement) {
  static sm_heap *current_heap = NULL;
  if (replacement != NULL) {
    sm_heap *temp = current_heap;
    current_heap  = replacement;
    return temp;
  }
  return current_heap;
}

// 'other' heap, also known as 'to' heap
sm_heap *sm_global_other_heap(sm_heap *replacement) {
  static sm_heap *other_heap = NULL;
  if (replacement != NULL) {
    sm_heap *temp = other_heap;
    other_heap    = replacement;
    return temp;
  }
  return other_heap;
}

// tracking number of garbage collections
// instead of NULL, use 0
int sm_gc_count(int increase) {
  static int gc_count = 0;
  gc_count += increase;
  return gc_count;
}

// Replacement is ignored if less than 1.
// New capacity assumes this increment:
// new_cap = old_cap * growth_factor + 1;
// Lowest possible value for replacement setting is 1.
double sm_global_growth_factor(double replacement) {
  static double factor = 1.25;
  if (replacement >= 1.0) {
    int previous_factor = factor;
    factor              = replacement;
    return previous_factor;
  }
  return factor;
}

// the global space array sorted by size
sm_space_array *sm_global_space_array(sm_space_array *replacement) {
  static sm_space_array *spaces = NULL;
  if (replacement != NULL) {
    sm_space_array *temp = spaces;
    spaces               = replacement;
    return temp;
  }
  return spaces;
}

// the global object stack
sm_expr *sm_global_obj_stack(sm_expr *replacement) {
  static sm_expr *expr = NULL;
  if (replacement != NULL) {
    sm_expr *temp = expr;
    expr          = replacement;
    return temp;
  }
  return expr;
}

// primitive_names. read only
char *sm_global_fn_name(unsigned short int which) {
  static char *response[] = {"+",   "-",    "*",    "/",    "sqrt", "sin", "cos",
                             "tan", "sinh", "cosh", "tanh", "^",    "csc", "sec",
                             "cot", "ln",   "exp",  "diff", "="};
  return response[which];
}

// corresponding string length of the string that would come from the sm_global_fn_name(which)
unsigned int sm_global_fn_name_len(unsigned short int which) {
  static long unsigned int response_len[] = {1, 1, 1, 1, 4, 3, 3, 3, 4, 4,
                                             4, 1, 3, 3, 3, 2, 3, 4, 1};
  return response_len[which];
}

unsigned int sm_global_num_fns() {
  static int num_fns = 19;
  return num_fns;
}
