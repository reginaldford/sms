// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "sms.h"
#include "../bison_flex/y.tab.h"

extern int        yylineno;
extern void      *memory_marker1;
extern bool       evaluating;
extern sm_object *return_obj;


void start_repl(sm_env *e);

// Prints intro
void print_intro() {
  printf("%s%sSymbolic Math System\n", sm_terminal_bg_color(SM_TERM_BLACK),
         sm_terminal_fg_color(SM_TERM_B_BLUE));
  printf("%sVersion %s%s\n", sm_terminal_fg_color(SM_TERM_B_WHITE),
         &(sms_global_version()->content), sm_terminal_reset());
}

// Initialize the heap, etc, if necessary
void check_init(sm_env *env, int num_args, char **argv) {
  if (env->initialized == false) {
    sm_global_environment(env);
    sm_init(env, num_args, argv);
    if (env->quiet_mode == false)
      print_intro();
  }
}

// Proper way to exit
void clean_exit(sm_env *env, int code) {
  if (env->initialized == true)
    sm_signal_exit(code);
  exit(code);
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
    // Before we eval, let's save a ptr to stack frame.
    memory_marker1 = __builtin_frame_address(0);
    evaluating     = true;
    sm_engine_eval(pr.parsed_object, *(sm_global_lex_stack(NULL)->top), NULL);
    evaluating = false;
  }
}

// Main function uses getopt from unistd.h
int main(int num_args, char *argv[]) {
  static struct sm_env env = {0}; // global environment structure
  opterr                   = 0;   // Disable error messages for unknown options
  int opt;
  while ((opt = getopt(num_args, argv, "qhm:e:s:i:c:l:pf:")) != -1) {
    switch (opt) {
    case 'h':
      printf("SMS Help\n");
      printf("Running sms with no flags will start the command line.\n");
      printf(" FLAG:                                             EXAMPLE:\n");
      printf("-h Help.                                            sms -h\n");
      printf("-q Quiet mode, does not print intro/outro.          sms -q -s script.sms\n");
      printf("-m Set memory usage. Units: kmgt. Default is 64m.   sms -m 150m\n");
      printf("-e Print the evaluation of an expression.           sms -e \"2*sin(PI/4)\"\n");
      printf("-s Run Script file.                                 sms -s script.sms\n");
      printf("-i Run a file, then start the REPL.                 sms -i script.sms\n");
      printf("-l Set the command history file. Disable with 'off' sms -l history.txt\n");
      printf("-c Custom argument. Accessed via _args. sms -c \"a single string\"\n");
      printf("-f Load an image from a file.                       sms -f img.fli\n");
      clean_exit(&env, 0);
      break;
    case 'f':
      if (env.initialized == false) {
        sm_global_environment(&env);
        sm_init_from_img(&env, num_args, argv, optarg);
        if (env.quiet_mode == false)
          print_intro();
      }
      break;
    case 'p':
      env.plain_mode = true;
      break;
    case 'l':
      if (!strcmp(optarg, "none")) {
        env.no_history_file = true;
        break;
      }
      env.history_file_len = strlen(optarg);
      sm_strncpy(env.history_file, optarg, env.history_file_len);
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
        char bytelength_str[16];
        sm_sprint_fancy_bytelength(bytelength_str, env.mem_bytes);
        printf("Custom Heap Size: %s\n", bytelength_str);
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
      memory_marker1 = __builtin_frame_address(0);
      evaluating     = true;
      sm_engine_eval(pr.parsed_object, *(sm_global_lex_stack(NULL)->top), NULL);
      sm_object *evaluated = return_obj;
      evaluating           = false;
      sm_string *response  = sm_object_to_string(evaluated);
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
      start_repl(&env);
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
    start_repl(&env);
  }
  clean_exit(&env, 0);
}
