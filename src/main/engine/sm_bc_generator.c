// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"

// Globals from sm_global.c
extern sm_heap   *sms_heap;
extern sm_heap   *sms_other_heap;
extern sm_symbol *sms_true;
extern sm_symbol *sms_false;

// Global true symbol
#define IS_TRUE (x)((void *)x == (void *)sms_true)
#define IS_FALSE(x) ((void *)x == (void *)sms_false)

// Generate a bytecode block which evaluates the input
sm_bc_block *sm_bc_generate(sm_object *input) {
  sm_string *bc_str = sm_new_string_manual(16);
  char      *bc     = &bc_str->content;
  int32_t    pos    = 0;
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
  case SM_ERROR_TYPE: {
  }
  default:
    printf("unrecognized object\n");
  } // end of switch
  return sm_new_bc_block(NULL, pos, bc);
}
