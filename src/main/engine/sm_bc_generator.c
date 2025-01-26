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
// VM globals
extern sm_stack2 *sms_stack;
extern sm_stack2 *sms_cx_stack;

typedef enum {
  SM_OP_PUSH,
  SM_OP_POP,
  SM_OP_DUP,
  SM_OP_TOP,
  SM_OP_IS_EMPTY,
  SM_OP_ADD,
  SM_OP_RETURN
} SM_OP;

// Generate a bytecode block which evaluates the input
sm_bc_block *sm_bc_generate(sm_object *input) {
  sm_string *bc_str = sm_new_string_manual(16);
  char      *bc     = &bc_str->content;
  int        pos    = 0;
  switch (input->my_type) {
  case SM_EXPR_TYPE: {
    sm_expr *sme = (sm_expr *)input;
    short    op  = sme->op;
    switch (op) {
    case SM_DATE_EXPR: {
      // YOU ARE HERE
      // bc[pos++] = 'd';
      break;
    }
    case SM_RUNTIME_META_EXPR: {
    }
    default:
      printf("unknown expression\n");
      break;
    } // End of expr case
  }
  case SM_META_TYPE: {
  }
  case SM_SELF_TYPE: {
  }
  case SM_PRIMITIVE_TYPE: {
  }
  case SM_SYMBOL_TYPE: {
  }
  case SM_LOCAL_TYPE: {
  }
  case SM_ERR_TYPE: {
  }
  default:
    printf("unrecognized object\n");
  } // end of switch
  return sm_new_bc_block(NULL, pos, bc);
}
