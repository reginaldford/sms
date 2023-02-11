// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "sms.h"

int main() {
  // Initialize global variables etc.
  sm_init();
  // Introduction and prompt
  printf("Symbolic Math System\n");
  printf("Version 0.135\n");
  while (true) {
    // Prompt
    sm_terminal_prompt();
    // Read
    sm_parse_result pr = sm_parse();
    if (pr.return_val == 0) {
      // Evaluate
      sm_object *result = sm_engine_eval(pr.parsed_object, *(sm_global_lex_stack(NULL)->top));
      // Print
      sm_string *result_str = sm_object_to_string(result);
      printf("%s\n", &(result_str->content));
      // Cleanup
      sm_garbage_collect();
    } else {
      printf("Error: parser returned %i\n", pr.return_val);
    }
  }
  // Exit gracefully.
  sm_signal_handler(SIGQUIT);
}
