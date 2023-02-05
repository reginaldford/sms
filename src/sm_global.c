// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

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

// the global lexical stack
sm_stack *sm_global_lex_stack(sm_stack *replacement) {
  static sm_stack *lex_stack = NULL;
  if (replacement != NULL) {
    sm_stack *temp = lex_stack;
    lex_stack      = replacement;
    return temp;
  }
  return lex_stack;
}

// primitive_names. read only
char *sm_global_fn_name(unsigned short int which) {
  const unsigned short int num_functions = sm_global_num_fns();
  static char *response[] = {"+",    "-",    "*",  "/",   "sqrt", "sin", "cos", "tan", "sinh",
                             "cosh", "tanh", "^",  "csc", "sec",  "cot", "ln",  "exp", "abs",
                             "diff", "=",    "==", "<",   ">",    "if",  "if",  "let", "?"};
  if (which >= num_functions) {
    return response[num_functions];
  }
  return response[which];
}

// corresponding string length of the string that would come from the sm_global_fn_name(which)
unsigned int sm_global_fn_name_len(unsigned short int which) {
  static long unsigned int response_len[] = {1, 1, 1, 1, 4, 3, 3, 3, 4, 4, 4, 1, 3,
                                             3, 3, 2, 3, 3, 4, 1, 2, 1, 1, 2, 2, 3};
  if (which >= sm_global_num_fns()) {
    return 1; // "?"
  }
  return response_len[which];
}

unsigned int sm_global_num_fns() {
  static const unsigned short int num_fns = 26;
  return num_fns;
}

// list of parent objects
// stored outside of heap and used for gc
sm_expr *sm_global_parents(sm_expr *replacement) {
  const int       initial_capacity = 100;
  static sm_expr *parents          = NULL;
  if (replacement != NULL) {
    sm_expr *previous = parents;
    parents           = replacement;
    return previous;
  }
  if (parents == NULL) {
    sm_expr *parents  = (sm_expr *)malloc(sizeof(sm_expr) + sizeof(void *) * initial_capacity);
    parents->my_type  = sm_expr_type;
    parents->op       = sm_siblings;
    parents->size     = 0;
    parents->capacity = initial_capacity;
    return parents;
  }
  return parents;
}
