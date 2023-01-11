// This project is licensed under the BSD 2 clause license. See LICENSE.txt for more information.

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
  } else {
    return global_context;
  }
}

// the current memory heap
sm_memory_heap *sm_global_current_heap(sm_memory_heap *replacement) {
  static sm_memory_heap *current_heap = NULL;
  if (replacement != NULL) {
    sm_memory_heap *temp = current_heap;
    current_heap         = replacement;
    return temp;
  } else {
    return current_heap;
  }
}

// the 'other' memory heap
sm_memory_heap *sm_global_other_heap(sm_memory_heap *replacement) {
  static sm_memory_heap *other_heap = NULL;
  if (replacement != NULL) {
    sm_memory_heap *temp = other_heap;
    other_heap           = replacement;
    return temp;
  } else {
    return other_heap;
  }
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
  } else {
    return spaces;
  }
}
