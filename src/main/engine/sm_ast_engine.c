// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"

// Globals from sm_global.c
extern sm_heap_set *sms_all_heaps;
extern sm_heap     *sms_heap;
extern sm_heap     *sms_other_heap;
extern sm_heap     *sms_symbol_heap;
extern sm_heap     *sms_symbol_name_heap;
extern sm_symbol   *sms_true;
extern sm_symbol   *sms_false;
extern sm_object   *return_obj;
// 2 virtual machine  stacks
extern sm_stack2 *sms_stack;
extern sm_stack2 *sms_cx_stack;

// Convenience functions for the booleans
#define IS_TRUE(x) ((void *)x == (void *)sms_true)
#define IS_FALSE(x) ((void *)x == (void *)sms_false)

// Old Recursive engine
inline void sm_engine_eval(sm_object *input, sm_cx *current_cx, sm_expr *sf) {
  printf("This eval func is depricated\n");
}

struct sm_vmi {
  size_t instruction;
};

struct sm_compiled {
  size_t         length;
  struct sm_vmi *content;
};

struct sm_compiled *sm_compile(sm_object *ast) {
  // return stuff
  return (struct sm_compiled *)NULL;
}

void sm_eval_compiled(struct sm_compiled *cmds) {}
