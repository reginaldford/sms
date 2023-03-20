// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

// Denotes the type of expression
enum SM_EXPR_TYPE {
  SM_PLUS_EXPR,
  SM_MINUS_EXPR,
  SM_TIMES_EXPR,
  SM_DIVIDE_EXPR,
  SM_SQRT_EXPR,
  SM_SIN_EXPR,
  SM_COS_EXPR,
  SM_TAN_EXPR,
  SM_SINH_EXPR,
  SM_COSH_EXPR,
  SM_TANH_EXPR,
  SM_POW_EXPR,
  SM_CSC_EXPR,
  SM_SEC_EXPR,
  SM_COT_EXPR,
  SM_CSCH_EXPR,
  SM_SECH_EXPR,
  SM_COTH_EXPR,
  SM_LN_EXPR,
  SM_EXP_EXPR,
  SM_ABS_EXPR,
  SM_DIFF_EXPR,
  SM_SIMP_EXPR,
  SM_ASSIGN_EXPR,
  SM_TEST_EQ_EXPR,
  SM_TEST_LT_EXPR,
  SM_TEST_GT_EXPR,
  SM_IF_EXPR,
  SM_IF_ELSE_EXPR,
  SM_LET_EXPR,
  SM_THEN_EXPR,
  SM_ARRAY_EXPR,
  SM_SIBLINGS_EXPR,
  SM_PRIM_EXPR,
  SM_PARAM_LIST_EXPR,
  SM_FUN_CALL_EXPR,
  SM_DOT_EXPR,
  SM_PARENT_EXPR,
  SM_INDEX_EXPR
};

// Head of an expression
// Assumed to have size of pointers following the structure
typedef struct sm_expr {
  short int         my_type;
  enum SM_EXPR_TYPE op;
  unsigned int      capacity;
  unsigned int      size;
} sm_expr;

sm_expr *sm_new_expr_n(enum SM_EXPR_TYPE op1, unsigned int size, unsigned int capacity);
sm_expr *sm_new_expr(enum SM_EXPR_TYPE op1, sm_object *arg);
sm_expr *sm_expr_append(sm_expr *expr, sm_object *arg);
sm_expr *sm_new_expr_2(enum SM_EXPR_TYPE op1, sm_object *arg1, sm_object *arg2);
sm_expr *sm_new_expr_3(enum SM_EXPR_TYPE op1, sm_object *arg1, sm_object *arg2, sm_object *arg3);
unsigned int sm_prefix_sprint(sm_expr *self, char *buffer, bool fake);
unsigned int sm_infix_sprint(sm_expr *expr, char *buffer, bool fake);
sm_string   *sm_expr_to_string(sm_expr *expr);
unsigned int sm_expr_sprint(sm_expr *self, char *buffer, bool fake);
sm_expr     *sm_expr_set_arg(sm_expr *expr, unsigned int index, sm_object *num);
sm_object   *sm_expr_get_arg(sm_expr *expr, unsigned int index);
bool         sm_is_infix(enum SM_EXPR_TYPE op);
sm_object   *sm_expr_pop(sm_expr *sme);
sm_object   *sm_expr_pop_recycle(sm_expr *sme);
