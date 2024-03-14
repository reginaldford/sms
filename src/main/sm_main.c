// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "sms.h"
#include "unistd.h"
#include "../bison_flex/y.tab.h"

extern int yylineno;

// Prints intro
void print_intro() {
  printf("%s%sSymbolic Math System\n", sm_terminal_bg_color(SM_TERM_BLACK),
         sm_terminal_fg_color(SM_TERM_B_BLUE));
  printf("%sVersion 0.21%s\n", sm_terminal_fg_color(SM_TERM_B_WHITE), sm_terminal_reset());
}

// Initialize the heap, etc, if necessary
void check_init(sm_env *env, int num_args, char **argv) {
  if (env->initialized == false) {
    if (env->quiet_mode == false)
      print_intro();
    sm_init(env, num_args, argv);
    sm_global_environment(env);
  }
}

// Proper way to exit
void clean_exit(sm_env *env, int code) {
  if (env->initialized == true)
    sm_signal_exit(code);
  exit(code);
}

// Run user command line
void start_repl() {
  // Reset the line counter
  yylineno = 1;
  // Read, Evaluate, Print Loop
  while (true) {
    // Prompt
    sm_terminal_print_prompt();
    sm_parse_result pr = sm_parse_file("/dev/stdin");
    // Read
    if (pr.return_val == 0 && pr.parsed_object != NULL) {
      // Evaluate
      sm_object *result = sm_engine_eval(pr.parsed_object, *(sm_global_lex_stack(NULL)->top), NULL);
      // Print
      sm_string *result_str = sm_object_to_string(result);
      printf("%s%s%s", sm_terminal_fg_color(SM_TERM_B_WHITE), &(result_str->content),
             sm_terminal_reset());
      // Cleanup
      sm_garbage_collect();
      // Count this as a line
      yylineno++;
    } else {
      printf("Error: Parser returned %i\n", pr.return_val);
      if (!pr.parsed_object)
        printf("Nothing was parsed.");
    }
  }
}

// Run the file
void run_file(char *file_path, sm_env *env) {
  sm_parse_result pr;
  yylineno = 1; // resetting the line count
  pr       = sm_parse_file(file_path);
  if (pr.return_val != 0) {
    printf("Error parsing the file. Parser returned %i\n", pr.return_val);
    clean_exit(env, 1);
  }
  if (pr.parsed_object != NULL) {
    sm_engine_eval(pr.parsed_object, *(sm_global_lex_stack(NULL)->top), NULL);
  }
}

// Main function uses getopt from unistd.h
int main(int num_args, char *argv[]) {
  static struct sm_env env = {0}; // global environment structure
  opterr                   = 0;   // Disable error messages for unknown options
  int opt;
  while ((opt = getopt(num_args, argv, "qhm:e:s:i:c:")) != -1) {
    switch (opt) {
    case 'h':
      printf("SMS Help\n");
      printf("Running sms with no flags will start the command line.\n");
      printf(" Flag:                                           Example:\n");
      printf("-h Help.                                          sms -h\n");
      printf("-q Quiet mode, does not print intro/outro.        sms -q -s script.sms\n");
      printf("-m Set memory usage. Units: kmgt. Default is 64m. sms -m 150m\n");
      printf("-e Print the evaluation of an expression.         sms -e \"2*sin(PI/4)\"\n");
      printf("-s Run Script file.                               sms -s script.sms\n");
      printf("-i Run a file, then start the REPL.               sms -i script.sms\n");
      printf("-c Custom argument. Accessed via _args.           sms -c \"a single string\"\n");
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
        "Value must be within range 2.5k to 4t (2.5 kilobytes to 4 terrabytes).";
      for (uint64_t i = 0; i < strlen(optarg); i++) {
        env.mem_str[i] = optarg[i];
      }
      env.mem_bytes = sm_bytelength_parse(env.mem_str, strlen(optarg));
      // SMS needs at least 2.5k (good luck with that) and might work with up to 4 terrabytes
      // (untested) For very basic programs, 1m is usually fine.
      if ((env.mem_bytes < 2.5 * 1024) || (env.mem_bytes > 4 * 10E12)) {
        printf("Invalid memory heap size: %s\n", env.mem_str);
        printf("%s\n", valid_values);
        printf("Try -h for help.\n");
        clean_exit(&env, 1);
      }
      if (env.quiet_mode == false) {
        printf("Custom Heap Size: ");
        printf("%lld Bytes (", (long long)env.mem_bytes);
        sm_print_fancy_bytelength(env.mem_bytes);
        printf(")\n");
      }
      break;
    }
    case 'e': {
      int optarg_len = strlen(optarg);
      check_init(&env, num_args, argv);
      sm_strncpy(env.eval_cmd, optarg, optarg_len);
      env.eval_cmd[optarg_len++] = ';';
      env.eval_cmd[optarg_len++] = '\0';
      env.eval_cmd_len           = optarg_len;
      if (env.quiet_mode == false)
        printf("Parsing: %s\n", env.eval_cmd);
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
      clean_exit(&env, 0);
      break;
    }
    case 's':
      check_init(&env, num_args, argv);
      sm_strncpy(&(env.script_fp[0]), optarg, strlen(optarg));
      run_file(optarg, &env);
      break;
    case 'i':
      check_init(&env, num_args, argv);
      sm_strncpy(env.script_fp, optarg, strlen(optarg));
      if (env.quiet_mode == false)
        printf("Loading: %s... \n", optarg);
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
  if (optind < num_args) { // Assuming the last arg is a filename
    char *input_file = argv[optind];
    check_init(&env, num_args, argv);
    sm_strncpy(&(env.script_fp[0]), input_file, strlen(input_file));
    run_file(input_file, &env);
  } else { // No filename provided
    check_init(&env, num_args, argv);
    start_repl();
  }
  clean_exit(&env, 0);
}
