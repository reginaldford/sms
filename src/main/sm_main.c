// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "sms.h"
#include "sm_main.h"
#include "../bison_flex/y.tab.h"

// Run user comaand line
void start_repl() {
  // Read, Evaluate, Print Loop
  while (true) {
    // Prompt
    sm_terminal_prompt();
    // Read
    sm_parse_result pr = sm_parse_stdin();
    if (pr.return_val == 0 && pr.parsed_object != NULL) {
      // Evaluate
      sm_object *result = sm_engine_eval(pr.parsed_object, *(sm_global_lex_stack(NULL)->top), NULL);
      // Print
      sm_string *result_str = sm_object_to_string(result);
      printf("%s\n", &(result_str->content));
      // Cleanup
      sm_garbage_collect();
    } else {
      printf("Error: parser returned %i\n", pr.return_val);
    }
  }
}

// If init is true , run file at options.init_fp
// If init is false, run file at options.script_fp
void run_file(sm_options *options, bool init) {
  sm_parse_result pr;
  sm_object      *last_parsed_obj = NULL;
  char           *file_path;
  if (init)
    file_path = options->init_fp;
  else
    file_path = options->script_fp;
  pr = sm_parse_file(file_path);
  do {
    if (pr.return_val != 0) {
      printf("Error parsing the file. Parser returned %i\n", pr.return_val);
      sm_signal_handler(SIGQUIT);
    }

    if (pr.parsed_object != NULL) {
      sm_object *evaluated =
        sm_engine_eval(pr.parsed_object, *(sm_global_lex_stack(NULL)->top), NULL);
      sm_string *response = sm_object_to_string(evaluated);
      printf("%s\n", &(response->content));
      last_parsed_obj = pr.parsed_object;
    }
    pr = sm_parse_more();
  } while (pr.parsed_object != last_parsed_obj);
  sm_parse_done();
  if (!init)
    sm_signal_handler(SIGQUIT);
}

// Main entry point
// Can run files, start REPL, or both
int main(int num_args, char **argv) {
  printf("Symbolic Math System\n");
  printf("Version 0.135\n");
  // Process command line args
  sm_global_options(sm_process_args(num_args, argv));
  sm_options *options = sm_global_options(NULL);
  sm_init(options);
  if (options->init_flag) {
    printf("Initializing with: %s... \n", options->init_fp);
    run_file(options, true);
  }
  if (options->script_flag) {
    printf("Running: %s... \n", options->script_fp);
    run_file(options, false);
  }
  if (options->eval_flag) {
    sm_parse_result pr = sm_parse_cstr(options->eval_cmd, options->eval_cmd_len);
    if (pr.return_val != 0) {
      printf("Parsing failed.\n");
      sm_signal_handler(SIGQUIT);
    }
    if (pr.parsed_object == NULL) {
      printf("Parser returned nothing.\n");
      sm_signal_handler(SIGQUIT);
    }
    sm_object *evaluated =
      sm_engine_eval(pr.parsed_object, *(sm_global_lex_stack(NULL)->top), NULL);
    sm_string *response = sm_object_to_string(evaluated);
    printf("%s\n", &(response->content));
    fflush(stdout);
    sm_signal_handler(SIGQUIT);
  }
  start_repl();
  sm_signal_handler(SIGQUIT);
}
