// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "sms.h"

void sm_init() {
  // Register the signal handler
  sm_register_signals();

  // Initialize the current mem heap
  sm_global_current_heap(sm_new_heap(1024 * 1024 * 50));

  // Initialize the global space arrays
  sm_global_space_array(sm_new_space_array(0, 100));

  // Initialize the lexical stack
  sm_global_lex_stack(sm_new_stack(100));

  // Initialize the global context
  sm_stack_push(sm_global_lex_stack(NULL), sm_new_context(0, 0, NULL));
}