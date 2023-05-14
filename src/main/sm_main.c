// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "sms.h"
#include "unistd.h"
#include "../bison_flex/y.tab.h"

extern int yylineno;

// Prints intro
void print_intro() {
  printf("Symbolic Math System\n");
  printf("Version 0.160\n");
}

// Initialize the heap, etc, if necessary
void check_init(sm_env *env) {
  if (env->initialized == false) {
    if (env->quiet_mode == false)
      print_intro();
    sm_init(env);
    env->initialized = true;
    sm_global_environment(env);
  }
}

// Proper way to exit
void clean_exit(sm_env *env, int code) {
  if (env->initialized == true) {
    sm_signal_exit(code);
  }
  exit(code);
}

// Run user command line
void start_repl() {
  // Reset the line counter
  yylineno = 1;
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
      // Count this as a line
      yylineno++;
    } else {
      printf("Error: parser returned %i\n", pr.return_val);
    }
  }
}

// If init is true , run file at env.init_fp then exit
// If init is false, run file at env.script_fp
void run_file(char *file_path, sm_env *env) {
  sm_parse_result pr;
  sm_object      *last_parsed_obj = NULL;
  file_path                       = env->script_fp;
  yylineno                        = 1; // resetting the line count
  pr                              = sm_parse_file(file_path);
  do {
    if (pr.return_val != 0) {
      printf("Error parsing the file. Parser returned %i\n", pr.return_val);
      clean_exit(env, 1);
    }
    if (pr.parsed_object != NULL) {
      sm_engine_eval(pr.parsed_object, *(sm_global_lex_stack(NULL)->top), NULL);
      last_parsed_obj = pr.parsed_object;
    }
    pr = sm_parse_more();
  } while (pr.parsed_object != last_parsed_obj);
  sm_parse_done();
}

// Main function uses getopt from unistd.h
int main(int num_args, char *argv[]) {
  static struct sm_env env;
  env.script_fp[0]  = '\0';
  env.script_fp_len = 0;
  env.eval_cmd[0]   = '\0';
  env.eval_cmd_len  = 0;
  env.gc            = true;
  env.print_stats   = true;
  env.mem_mbytes    = 50.0;
  env.initialized   = false;
  env.quiet_mode    = false;

  opterr = 0; // Disable error messages for unknown options
  int opt;
  while ((opt = getopt(num_args, argv, "qhm:e:s:i:")) != -1) {
    switch (opt) {
    case 'h':
      printf("SMS Help\n");
      printf("Running sms with no flags will start the command line.\n");
      printf(" Flag:                                      Example:\n");
      printf("-h Help.                                     sms -h\n");
      printf("-q Quiet mode, does not print intro/outro.   sms -q -s script.sms\n");
      printf("-m Set the heap size in MB. Default: 50.     sms -m 150\n");
      printf("-e Print the evaluation of an expression.    sms -e \"2*sin(PI/4)\"\n");
      printf("-s Run Script file.                          sms -s script.sms\n");
      printf("-i Run a file, then start the REPL.          sms -i script.sms\n");
      printf("\nIf the -m flag is used, it must be first.  sms -m 4 -s x1.sms -i x2.sms\n");
      printf("Some flags can be repeated and all flags are executed in order.\n");
      clean_exit(&env, 0);
      break;
    case 'q':
      env.quiet_mode = true;
      break;
    case 'm': {
      if (env.mem_flag) {
        printf("Error: Memory flag can only be set once.\n");
        clean_exit(&env, 1);
      }
      env.mem_flag = true;
      const char *valid_values =
        "Value must be in the range 0.01 to 4000000 inclusively (in Megabytes)";
      long unsigned int i = 0;
      for (; i < strlen(optarg); i++) {
        env.mem_str[i] = optarg[i];
      }
      env.mem_mbytes = atof(env.mem_str);
      if (env.mem_mbytes < 0.01 || env.mem_mbytes > 1000 * 4000) {
        printf("Invalid memory heap size: %s\n", env.mem_str);
        printf("%s\n", valid_values);
        printf("Try -h for help.\n");
        clean_exit(&env, 1);
      }
      if (env.quiet_mode == false) {
        printf("Custom Heap Size: ");
        if (env.mem_mbytes < 1)
          printf("%.16g KB\n", env.mem_mbytes * 1E3);
        else if (env.mem_mbytes < 1E4)
          printf("%.16g MB\n", env.mem_mbytes);
        else if (env.mem_mbytes >= 1E4 && env.mem_mbytes < 1E6)
          printf("%.16g GB\n", env.mem_mbytes / 1E3);
        else if (env.mem_mbytes >= 1E6)
          printf("%.16g TB\n", env.mem_mbytes / 1E6);
      }
      break;
    }
    case 'e': {
      int optarg_len = strlen(optarg);
      sm_strncpy(&(env.eval_cmd[0]), optarg, optarg_len);
      env.eval_cmd[optarg_len++] = ';';
      env.eval_cmd[optarg_len++] = '\0';
      env.eval_cmd_len           = optarg_len;
      check_init(&env);
      if (env.quiet_mode == false)
        printf("Evaluating: %s.\n", env.eval_cmd);
      sm_parse_result pr = sm_parse_cstr(env.eval_cmd, env.eval_cmd_len);
      if (pr.return_val != 0) {
        printf("Error: Parser failed and returned %i\n", pr.return_val);
        clean_exit(&env, 1);
      }
      if (pr.parsed_object == NULL) {
        printf("Parser returned nothing.\n");
        clean_exit(&env, 1);
      }
      sm_object *evaluated =
        sm_engine_eval(pr.parsed_object, *(sm_global_lex_stack(NULL)->top), NULL);
      sm_string *response = sm_object_to_string(evaluated);
      printf("%s\n", &(response->content));
      fflush(stdout);
      break;
    }
    case 's':
      sm_strncpy(&(env.script_fp[0]), optarg, strlen(optarg));
      check_init(&env);
      if (env.quiet_mode == false)
        printf("Running: %s... \n", env.script_fp);
      run_file(optarg, &env);
      break;
    case 'i':
      sm_strncpy(&(env.script_fp[0]), optarg, strlen(optarg));
      check_init(&env);
      if (env.quiet_mode == false)
        printf("Loading: %s... \n", env.script_fp);
      run_file(optarg, &env);
      start_repl();
      break;
    case '?':
      if (optopt == 'm') {
        fprintf(stderr, "Option -%c requires an argument.\n", optopt);
        clean_exit(&env, 1);
      } else if (isprint(optopt)) {
        fprintf(stderr, "Unknown option `-%c'.\n", optopt);
        clean_exit(&env, 1);
      } else {
        fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
        clean_exit(&env, 1);
      }
      break;
    default:
      break;
    }
  }
  if (optind < num_args) {
    char *input_file = argv[optind];
    sm_strncpy(&(env.script_fp[0]), input_file, strlen(input_file));
    check_init(&env);
    if (env.quiet_mode == false)
      printf("Running: %s... \n", env.script_fp);
    run_file(input_file, &env);
  } else if (env.initialized == false) {
    check_init(&env);
    start_repl();
  }
  clean_exit(&env, 0);
}
