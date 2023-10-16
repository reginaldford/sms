// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "sms.h"

extern sm_heap *sms_heap;

void sm_init(sm_env *env, int num_args, char **argv, bool quiet) {
  // Default (inner) environment variables
  int mem_mbytes     = 50;
  env->script_fp[0]  = '\0';
  env->script_fp_len = 0;
  env->eval_cmd[0]   = '\0';
  env->eval_cmd_len  = 0;
  env->gc            = true;
  env->num_args      = num_args;
  env->args          = argv;
  env->quiet_mode    = quiet;

  if (env->mem_flag)
    mem_mbytes = env->mem_mbytes; // mem_mbytes overrides
  else
    env->mem_mbytes = mem_mbytes;

  // Register the signal handler
  sm_register_signals();

  // Initialize the current memory heap
  // During first gc, a second heap of the same size will be allocated.
  sms_heap = sm_new_heap(mem_mbytes * 1024 * 1024 / 2);

  // Initialize the global space arrays
  sm_global_space_array(sm_new_space_array(0, 100));

  // Initialize the lexical stack
  sm_global_lex_stack(sm_new_stack(100));

  // Build the global context's parent
  sm_cx *parent_cx = sm_new_cx(NULL);
  sm_cx_let(parent_cx, "PI", 2, (sm_object *)sm_new_double(3.14159265358979323846));
  // Add program args if available
  if (env) {
    sm_expr *args = sm_new_expr_n(SM_ARRAY_EXPR, env->num_args, env->num_args);
    for (int i = 0; i < env->num_args; i++) {
      sm_expr_set_arg(args, i, (sm_object *)sm_new_string(strlen(env->args[i]), env->args[i]));
    }
    sm_cx_let(parent_cx, "_args", 5, (sm_object *)args);
  }
  // true singleton
  sm_symbol *true_sym = sm_new_symbol_manual(sm_new_string(4, "true"));
  sm_cx_let(parent_cx, "true", 4, (sm_object *)true_sym);
  sms_true = true_sym;

  // false singleton
  sm_symbol *false_sym = sm_new_symbol_manual(sm_new_string(5, "false"));
  sm_cx_let(parent_cx, "false", 5, (sm_object *)false_sym);
  sms_false = false_sym;

  // Initialize the global context
  sm_cx *scratch = sm_new_cx(parent_cx);
  sm_stack_push(sm_global_lex_stack(NULL), scratch);

  // _scratch global cx variable
  sm_cx_let(parent_cx, "_scratch", 8, (sm_object *)scratch);

  // Done
  env->initialized = true;
}
