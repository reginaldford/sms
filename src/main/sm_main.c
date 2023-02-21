// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "sms.h"
#include "sm_main.h"

void process_args(int num_args, char **argv) {
  // static char             *valid_flags = "-e -g -h -i -m -p -s";
  static char             *valid_flags = "-s";
  static struct sm_options os;
  os.script_flag   = false;
  os.script_fp[0]  = '\0';
  os.script_fp_len = 0;
  os.always_gc     = false;
  os.print_stats   = true;
  os.eval_flag     = false;
  os.eval_cmd[0]   = '\0';
  os.eval_cmd_len  = 0;
  os.init_flag     = false;
  os.init_fp[0]    = '\0';
  os.init_fp_len   = 0;

  for (int current_arg = 1; current_arg < num_args; current_arg++) {
    if (argv[current_arg][0] != '-') {
      printf("Invalid flag: %c\n", argv[current_arg][1]);
      printf("Every flag begins with -. Example: sms -e 2+2\n");
      sm_signal_handler(SIGQUIT);
    }
    switch (argv[current_arg][1]) {
    case 'i': {
      os.init_flag = true;
    }
    case 'e': {
      // Store the rest of the args as sms code
      os.eval_flag = true;
      current_arg++;
      long unsigned int i = 0;
      for (; current_arg < num_args; current_arg++) {
        for (size_t arg_pos = 0; arg_pos < strlen(argv[current_arg]); i++) {
          os.eval_cmd[i] = argv[current_arg][arg_pos++];
        }
      }
      break;
    }
    case 'g': {
    }
    case 'h': {
      // Help
    }
    case 'm': {
    }
    case 'p': {
    }
    case 's': {
      // Script
      os.script_flag = true;
      for (long unsigned int i = 0; i < strlen(argv[current_arg + 1]); i++) {
        os.script_fp[i] = argv[current_arg + 1][i];
      }
      current_arg++;
      break;
    }

    default: {
      printf("Invalid flag. Valid flags: %s\n", valid_flags);
      // printf("Try -h for help.\n");
      sm_signal_handler(SIGQUIT);
    }
    }
  }
  sm_global_options(&os);
}

void start_repl() {
  // Read, Evaluate, Print Loop
  while (true) {
    // Prompt
    sm_terminal_prompt();
    // Read
    sm_parse_result pr = sm_parse();
    if (pr.return_val == 0) {
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

int main(int num_args, char **argv) {
  // Introduction and prompt
  printf("Symbolic Math System\n");
  printf("Version 0.135\n");
  // Initialize global variables etc.
  sm_init();
  process_args(num_args, argv);
  sm_options os = *sm_global_options(NULL);
  if (os.init_flag) {
    // parse the init file based on this filepath
    // os.env_file;
  }
  if (os.script_flag) {
    printf("Running: %s... \n", os.script_fp);
    freopen(os.script_fp, "r", stdin);
    sm_parse_result pr = sm_parse();
    if (pr.return_val != 0) {
      printf("Error parsing the file. Parser returned %i\n", pr.return_val);
      sm_signal_handler(SIGQUIT);
    }
    sm_object *evaluated =
      sm_engine_eval(pr.parsed_object, *(sm_global_lex_stack(NULL)->top), NULL);
    sm_string *response = sm_object_to_string(evaluated);
    printf("%s\n", &(response->content));
  }
  if (os.eval_flag) {
    // TODO: Evaluate sm_options.eval_cmd
    printf("Command line execution feature is not implemented yet.\n");
  }
  start_repl();
  sm_signal_handler(SIGQUIT);
}
