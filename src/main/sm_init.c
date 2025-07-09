// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "sms.h"

extern struct sm_heap_set *sms_all_heaps;
extern struct sm_heap     *sms_heap;
extern struct sm_heap     *sms_symbol_heap;
extern struct sm_heap     *sms_symbol_name_heap;
extern uint32_t            sms_num_symbols;
extern const char         *sms_version;
extern const int           sms_version_len;
extern bool                evaluating;
extern sm_stack2          *sms_stack;
extern sm_stack2          *sms_cx_stack;
extern sm_stack2          *sms_sf;

void sm_init(sm_env *env, int num_args, char **argv) {
  // Set version number. Major.Minor.Patch
  char *sms_version     = SMS_VERSION;
  int   sms_version_len = SMS_VERSION_LEN;
  sm_strncpy(env->version, sms_version, sms_version_len);
  env->version_len = sms_version_len;
  // Register the signal handler
  sm_register_signals();
  sms_all_heaps = sm_new_heap_set(1024, 0, malloc(sizeof(sm_heap *) * 1024));
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
    sm_string *home_dir = sm_global_home_directory();
    if (home_dir) {
      sprintf(env->history_file, "%s/.sms_history", &home_dir->content);
      env->history_file_len = strlen(env->history_file);
    }
  }

  // Symbol related heap allocations
  sms_symbol_heap      = sm_new_heap(1024 * 512, true);
  sms_symbol_name_heap = sm_new_heap(1024 * 1024, false);
  sms_num_symbols      = 0;

  // Initialize the current memory heap
  // During first gc, a second heap of the same size will be allocated.
  sms_heap = sm_new_heap(mem_bytes / 2, true);

  // Virual machine stacks
  sms_stack    = sm_new_stack2(1024);
  sms_cx_stack = sm_new_stack2(1024);
  sms_sf       = sm_new_stack2(1024);

  // Initialize the lexical stack
  sm_global_lex_stack(sm_new_stack(128));

  // Build the global context's parent
  sm_cx     *parent_cx = sm_new_cx(NULL);
  sm_symbol *pi_sym    = sm_new_symbol("PI", 2);
  sm_cx_let(parent_cx, pi_sym, (sm_object *)sm_new_f64(3.14159265358979323846));
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
  sms_cx_stack = sm_push(sms_cx_stack, (sm_object *)scratch);

  // _scratch global cx variable
  sm_cx_let(parent_cx, sm_new_symbol("_scratch", 8), (sm_object *)scratch);

  // linenoise setup
  linenoiseSetCompletionCallback(sm_terminal_completion);

  // Set Evaluating global
  evaluating = false;

  // Done
  env->initialized = true;
}

// Load an image
void sm_init_from_img(sm_env *env, int num_args, char **argv, const char *img_file_path) {
  sm_img img;
  printf("file path: %s\n", img_file_path);
  printf("len path: %lu\n", strlen(img_file_path));
  FILE *file = fopen(img_file_path, "rb");
  printf("file is %p\n", file);
  printf("sizeof img is %lu\n", sizeof(sm_img));
  // Image will have dynamic size
  // YOU ARE HERE
  if (!file || fread(&img, sizeof(sm_img), 1, file) != 1) {
    fprintf(stderr, "Failed to open or read image file: %s\n", img_file_path);
    if (file)
      fclose(file);
    exit(EXIT_FAILURE);
  }
  fclose(file);

  // Initialize environment from image data
  *env          = img.env;
  env->num_args = num_args;
  env->args     = argv;
  sm_strncpy(env->version, SMS_VERSION, SMS_VERSION_LEN);
  env->version_len = SMS_VERSION_LEN;
  sm_register_signals();

  // Restore global objects
  sms_heap             = &img.sms_heap;
  sms_other_heap       = sm_new_heap(sms_heap->capacity, true); // Assuming a second heap for GC
  sms_symbol_heap      = &img.sms_symbol_heap;
  sms_symbol_name_heap = &img.sms_symbol_name_heap;
  sms_true             = &img.sms_true;
  sms_false            = &img.sms_false;
  sms_num_symbols      = img.num_symbols; // Correctly restore the symbol count

  // Initialize heap set and add restored heaps
  sms_all_heaps = sm_new_heap_set(1024, 0, malloc(sizeof(void *) * 1024)); // Initialize heap set
  sm_heap_set_add(sms_all_heaps, sms_heap);
  sm_heap_set_add(sms_all_heaps, sms_other_heap);
  sm_heap_set_add(sms_all_heaps, sms_symbol_heap);
  sm_heap_set_add(sms_all_heaps, sms_symbol_name_heap);

  // Initialize lexical stack
  sm_global_lex_stack(sm_new_stack(128));

  // Setup terminal completion
  linenoiseSetCompletionCallback(sm_terminal_completion);
  env->initialized = true;
}
