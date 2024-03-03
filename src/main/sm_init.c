// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "sms.h"
#include "../../submodules/bounce/src/bounce.h"

extern struct sm_heap *sms_heap;
extern struct sm_heap *sms_symbol_heap;
extern struct sm_heap *sms_symbol_name_heap;
extern uint32_t        sms_num_symbols;
extern uint8_t * sms_key;
extern uint32_t sms_ks1;
extern uint32_t sms_ks2;
extern uint8_t sms_sub_table[256];
 
void sm_init(sm_env *env, int num_args, char **argv) {
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

  // Symbol related heap allocations
  sms_symbol_heap      = sm_new_heap(1024 * 512);
  sms_symbol_name_heap = sm_new_heap(1024 * 1024);
  sms_num_symbols      = 0;
  // Initialize the current memory heap
  // During first gc, a second heap of the same size will be allocated.
  sms_heap = sm_new_heap(mem_bytes / 2);
  // Store bounce encryption prereqs
  // Globals: sms_key, sms_ks1, sms_ks2, and sms_sub_table
  bounceReadFileResult brfr         = bounceReadFile("sms_key");
  if (!brfr.fileExists) {
    printf("SMS key file not found!\n");
    exit(1);
  }
  sms_key = brfr.fileContent;
  // Calculate and store the keySum
  sms_ks1 = bounceProcKeySum(sms_key);
  sms_ks2 = bounceProcKeySum(sms_key + 128);
  // Invertable Swap table generated from key
  bounceTableInit(sms_key, sms_sub_table);

  // Initialize the lexical stack
  sm_global_lex_stack(sm_new_stack(128));

  // Build the global context's parent
  sm_cx *parent_cx = sm_new_cx(NULL);
  sm_cx_let(parent_cx, "PI", 2, (sm_object *)sm_new_double(3.14159265358979323846));
  // Add program args if available
  if (env) {
    sm_expr *args = sm_new_expr_n(SM_ARRAY_EXPR, env->num_args, env->num_args);
    // Note that strlen is unavoidable here
    for (int i = 0; i < env->num_args; i++)
      sm_expr_set_arg(args, i, (sm_object *)sm_new_string(strlen(env->args[i]), env->args[i]));
    sm_cx_let(parent_cx, "_args", 5, (sm_object *)args);
  }
  // true singleton
  sm_symbol *true_sym = sm_new_symbol("true", 4);
  sm_cx_let(parent_cx, "true", 4, (sm_object *)true_sym);
  sms_true = true_sym;

  // false singleton
  sm_symbol *false_sym = sm_new_symbol("false", 5);
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
