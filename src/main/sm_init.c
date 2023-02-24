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

  // Initialize the global context
  sm_stack_push(sm_global_lex_stack(NULL), sm_new_context(0, 0, NULL));
}
