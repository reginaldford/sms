// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "sms.h"

// Stores a static structure with processed command line information
// or prints helping information for command line arguments and exits.
sm_options *sm_process_args(int num_args, char **argv) {
  static char             *valid_flags = "-e -h -i -s";
  static struct sm_options options;
  options.script_flag   = false;
  options.script_fp[0]  = '\0';
  options.script_fp_len = 0;
  options.env_flag      = false;
  options.env_fp[0]     = '\0';
  options.env_fp_len    = 0;
  options.always_gc     = false;
  options.print_stats   = true;
  options.eval_flag     = false;
  options.eval_cmd[0]   = '\0';
  options.eval_cmd_len  = 0;
  options.init_flag     = false;
  options.init_fp[0]    = '\0';
  options.init_fp_len   = 0;

  for (int current_arg = 1; current_arg < num_args; current_arg += 2) {
    if (argv[current_arg][0] != '-') {
      printf("Invalid flag: %c\n", argv[current_arg][0]);
      printf("Every flag begins with -. Example: sms -e 2+2\n");
      exit(-1);
    }
    switch (argv[current_arg][1]) {
    case 'i': {
      options.init_flag   = true;
      long unsigned int i = 0;
      for (; i < strlen(argv[current_arg + 1]); i++) {
        options.init_fp[i] = argv[current_arg + 1][i];
      }
      options.init_fp_len = i;
      break;
    }
    case 'e': {
      options.eval_flag   = true;
      long unsigned int i = 0;
      for (; current_arg + 1 < num_args; current_arg++) {
        for (size_t arg_pos = 0; arg_pos < strlen(argv[current_arg + 1]); i++) {
          options.eval_cmd[i] = argv[current_arg + 1][arg_pos++];
        }
      }
      options.eval_cmd[i++] = ';';
      options.eval_cmd[i]   = '\0';
      options.eval_cmd_len  = i;
      break;
    }
    case 'h': {
      printf("SMS Help\n");
      printf("-e to Execute a calculation:                 sms -e 2+2\n");
      printf("-h for Help:                                 sms -h\n");
      printf("-i to run Initial file then start the REPL:  sms -i script.sms\n");
      printf("-s to run Script file  then exit:            sms -s script.sms\n");
      exit(0);
    }
    case 's': {
      options.script_flag = true;
      long unsigned int i = 0;
      for (; i < strlen(argv[current_arg + 1]); i++) {
        options.script_fp[i] = argv[current_arg + 1][i];
      }
      options.script_fp_len = i;
      break;
    }
    default: {
      printf("Invalid flag: %c Valid flags: %s\n", argv[current_arg][1], valid_flags);
      printf("Try -h for help.\n");
      exit(-1);
    }
    }
  }
  return &options;
}
