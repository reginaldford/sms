// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "sms.h"

void sm_init(sm_options *options) {
  // Register the signal handler
  sm_register_signals();

  // Initialize the current mem heap
  unsigned int mem_mbytes = 50;
  if (options != NULL && options->mem_flag) {
    mem_mbytes = options->mem_mbytes;
    printf("Custom Heap Size: %i MB\n", mem_mbytes);
  }
  // Start with half of the heap size allocated.
  // During first gc, a second heap of the same size will be allocated.
  sm_global_current_heap(sm_new_heap(mem_mbytes * 1024 * 1024 / 2));

  // Initialize the global space arrays
  sm_global_space_array(sm_new_space_array(0, 100));

  // Initialize the lexical stack
  sm_global_lex_stack(sm_new_stack(100));

  // Build the global context's parent
  sm_context       *parent_cx = sm_new_context(3, 3, NULL);
  sm_context_entry *parents   = sm_context_entries(parent_cx);
  // The keys MUST be in strcmp order
  parents[0] = (sm_context_entry){.name  = sm_new_string(2, "PI"),
                                  .value = (sm_object *)sm_new_double(3.14159265358979323846)};
  parents[1] =
    (sm_context_entry){.name  = sm_new_string(5, "false"),
                       .value = (sm_object *)sm_new_meta(
                         (sm_object *)sm_new_symbol(sm_new_string(5, "false")), parent_cx)};

  parents[2] =
    (sm_context_entry){.name  = sm_new_string(4, "true"),
                       .value = (sm_object *)sm_new_meta(
                         (sm_object *)sm_new_symbol(sm_new_string(4, "true")), parent_cx)};

  // Initialize the global context
  sm_stack_push(sm_global_lex_stack(NULL), sm_new_context(0, 0, parent_cx));
}
