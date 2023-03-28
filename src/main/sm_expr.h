// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

// Denotes the type of expression
enum SM_EXPR_TYPE {
  SM_PLUS_EXPR,       // 0
  SM_MINUS_EXPR,      // 1
  SM_TIMES_EXPR,      // 2
  SM_DIVIDE_EXPR,     // 3
  SM_SQRT_EXPR,       // 4
  SM_SIN_EXPR,        // 5
  SM_COS_EXPR,        // 6
  SM_TAN_EXPR,        // 7
  SM_SINH_EXPR,       // 8
  SM_COSH_EXPR,       // 9
  SM_TANH_EXPR,       // 10
  SM_POW_EXPR,        // 11
  SM_CSC_EXPR,        // 12
  SM_SEC_EXPR,        // 13
  SM_COT_EXPR,        // 14
  SM_CSCH_EXPR,       // 15
  SM_SECH_EXPR,       // 16
  SM_COTH_EXPR,       // 17
  SM_LN_EXPR,         // 18
  SM_EXP_EXPR,        // 19
  SM_ABS_EXPR,        // 20
  SM_DIFF_EXPR,       // 21
  SM_SIMP_EXPR,       // 22
  SM_ASSIGN_EXPR,     // 23
  SM_TEST_EQ_EXPR,    // 24
  SM_TEST_LT_EXPR,    // 25
  SM_TEST_GT_EXPR,    // 26
  SM_IF_EXPR,         // 27
  SM_IF_ELSE_EXPR,    // 28
  SM_LET_EXPR,        // 29
  SM_THEN_EXPR,       // 30
  SM_ARRAY_EXPR,      // 31
  SM_SIBLINGS_EXPR,   // 32
  SM_PRIM_EXPR,       // 33
  SM_PARAM_LIST_EXPR, // 34
  SM_FUN_CALL_EXPR,   // 35
  SM_DOT_EXPR,        // 36
  SM_PARENT_EXPR,     // 37
  SM_SIZE_EXPR,       // 38
  SM_MAP_EXPR,        // 39
  SM_INDEX_EXPR,      // 40
  SM_WHILE_EXPR,      // 41
  SM_PRINT_EXPR,      // 42
  SM_TO_STRING_EXPR,  // 43
  SM_EVAL_EXPR,       // 43
  SM_UNKNOWN_EXPR     // 44
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
