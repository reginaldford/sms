// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "sms.h"

extern struct sm_heap *sms_heap;
extern struct sm_heap *sms_symbol_heap;
extern struct sm_heap *sms_symbol_name_heap;
extern uint32_t        sms_num_symbols;

void sm_init(sm_env *env, int num_args, char **argv) {
  // Set version number. Major.Minor.Patch
  char *sms_version     = "0.21.18";
  int   sms_version_len = 7;
  sm_strncpy(env->version, sms_version, sms_version_len);
  env->version_len = sms_version_len;
  // Register the signal handler
  sm_register_signals();
  // Default (inner) environment variables
  double mem_bytes   = 64 * 1024 * 1024;
  env->script_fp[0]  = '\0';
  env->script_fp_len = 0;
  env->eval_cmd[0]   = '\0';
  env->eval_cmd_len  = 0;
  env->gc            = true;
  env->num_args      = num_args;
  env->args          = argv;
  if (env->mem_flag)
    mem_bytes = env->mem_bytes; // mem_bytes overrides
  else
    env->mem_bytes = mem_bytes;
  // Using default history file
  if (!env->no_history_file && !env->history_file_len) {
    char       history_path[256];
    sm_string *home_dir = sm_global_home_directory();
    if (home_dir) {
      sprintf(env->history_file, "%s/.sms_history", &home_dir->content);
      env->history_file_len = strlen(env->history_file);
    }
  }

  // Symbol related heap allocations
  sms_symbol_heap      = sm_new_heap(1024 * 512);
  sms_symbol_name_heap = sm_new_heap(1024 * 1024);
  sms_num_symbols      = 0;

  // Initialize the current memory heap
  // During first gc, a second heap of the same size will be allocated.
  sms_heap = sm_new_heap(mem_bytes / 2);

  // Initialize the lexical stack
  sm_global_lex_stack(sm_new_stack(128));

  // Build the global context's parent
  sm_cx     *parent_cx = sm_new_cx(NULL);
  sm_symbol *pi_sym    = sm_new_symbol("PI", 2);
  sm_cx_let(parent_cx, pi_sym, (sm_object *)sm_new_double(3.14159265358979323846));
  // Add program args if available
  if (env) {
    sm_expr *args = sm_new_expr_n(SM_TUPLE_EXPR, env->num_args, env->num_args, NULL);
    // Note that strlen is unavoidable here
    for (int i = 0; i < env->num_args; i++)
      sm_expr_set_arg(args, i, (sm_object *)sm_new_string(strlen(env->args[i]), env->args[i]));
    sm_cx_let(parent_cx, sm_new_symbol("_args", 5), (sm_object *)args);
  }
  // true singleton
  sms_true = sm_new_symbol("true", 4);
  sm_cx_let(parent_cx, sms_true, (sm_object *)sms_true);

  // false singleton
  sms_false = sm_new_symbol("false", 5);
  sm_cx_let(parent_cx, sms_false, (sm_object *)sms_false);

  // Initialize the global context
  sm_cx *scratch = sm_new_cx(parent_cx);
  sm_stack_push(sm_global_lex_stack(NULL), scratch);

  // _scratch global cx variable
  sm_cx_let(parent_cx, sm_new_symbol("_scratch", 8), (sm_object *)scratch);

  // linenoise setup
  linenoiseSetCompletionCallback(sm_terminal_completion);

  // Done
  env->initialized = true;
}
