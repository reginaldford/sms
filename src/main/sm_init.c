// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "sms.h"

extern sm_heap *sms_heap;

void sm_init(sm_env *env) {
  // Register the signal handler
  sm_register_signals();

  // Initialize the current memory heap
  double mem_mbytes = 50;
  if (env != NULL)
    mem_mbytes = env->mem_mbytes;

  // Start with half of the heap size allocated.
  // During first gc, a second heap of the same size will be allocated.
  sms_heap = sm_new_heap(mem_mbytes * 1024 * 1024 / 2);

  // Initialize the global space arrays
  sm_global_space_array(sm_new_space_array(0, 100));

  // Initialize the lexical stack
  sm_global_lex_stack(sm_new_stack(100));

  // Build the global context's parent
  sm_cx *parent_cx = sm_new_cx(NULL);
  sm_cx_let(parent_cx, "PI", 2, (sm_object *)sm_new_double(3.14159265358979323846));

  // true singleton
  sm_symbol *true_sym = sm_new_symbol_manual(sm_new_string(4, "true"));
  sm_cx_let(parent_cx, "true", 4, (sm_object *)true_sym);
  sms_true = true_sym;

  // false singleton
  sm_symbol *false_sym = sm_new_symbol_manual(sm_new_string(5, "false"));
  sm_cx_let(parent_cx, "false", 5, (sm_object *)false_sym);
  sms_false = false_sym;

  // Initialize the global context
  sm_stack_push(sm_global_lex_stack(NULL), sm_new_cx(parent_cx));
}
