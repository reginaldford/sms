// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

// Denotes the type of expression
// Must be in the same order as sm_global_fn_name
enum SM_EXPR_TYPE {
  SM_EXIT_EXPR,
  SM_CLEAR_EXPR,
  SM_SELF_EXPR,
  SM_LET_EXPR,
  SM_ASSIGN_EXPR,
  SM_RM_EXPR,
  SM_DOT_EXPR,
  SM_PARENT_EXPR,
  SM_PLUS_EXPR,
  SM_MINUS_EXPR,
  SM_TIMES_EXPR,
  SM_DIVIDE_EXPR,
  SM_POW_EXPR,
  SM_SIN_EXPR,
  SM_COS_EXPR,
  SM_TAN_EXPR,
  SM_SINH_EXPR,
  SM_COSH_EXPR,
  SM_TANH_EXPR,
  SM_CSC_EXPR,
  SM_SEC_EXPR,
  SM_COT_EXPR,
  SM_CSCH_EXPR,
  SM_SECH_EXPR,
  SM_COTH_EXPR,
  SM_EXP_EXPR,
  SM_LN_EXPR,
  SM_SQRT_EXPR,
  SM_ABS_EXPR,
  SM_DIFF_EXPR,
  SM_SIMP_EXPR,
  SM_IF_EXPR,
  SM_IF_ELSE_EXPR,
  SM_EQEQ_EXPR,
  SM_LT_EXPR,
  SM_GT_EXPR,
  SM_LT_EQ_EXPR,
  SM_GT_EQ_EXPR,
  SM_THEN_EXPR,
  SM_ARRAY_EXPR,
  SM_SIZE_EXPR,
  SM_INDEX_EXPR,
  SM_MAP_EXPR,
  SM_WHILE_EXPR,
  SM_EVAL_EXPR,
  SM_PARSE_EXPR,
  SM_TO_STRING_EXPR,
  SM_PRINT_EXPR,
  SM_PRINTLN_EXPR,
  SM_INPUT_EXPR,
  SM_FILE_PARSE_EXPR,
  SM_FILE_READ_EXPR,
  SM_FILE_WRITE_EXPR,
  SM_RANDOM_EXPR,
  SM_ROUND_EXPR,
  SM_NOT_EXPR,
  SM_OR_EXPR,
  SM_STR_ESCAPE_EXPR,
  SM_STR_LEN_EXPR,
  SM_STR_FIND_EXPR,
  SM_STR_ADD_EXPR,
  SM_STR_DIVIDE_EXPR,
  SM_STR_PART_EXPR,
  SM_STR_UPPER_EXPR,
  SM_STR_LOWER_EXPR,
  SM_STR_ISNUM_EXPR,
  SM_FUN_CALL_EXPR,
  SM_PARAM_LIST_EXPR,
  SM_SIBLINGS_EXPR,
  SM_PRIM_EXPR,
  SM_DATE_STR_EXPR,
  SM_DATE_EXPR,
  SM_TIME_EXPR,
  SM_SLEEP_EXPR,
  SM_UNKNOWN_EXPR
};

// Head of an expression
// Assumed to have pointers following the structure for each argument
// Size denotes the number of trailing pointers.
typedef struct sm_expr {
  short int         my_type;
  enum SM_EXPR_TYPE op;
  unsigned int      capacity;
  unsigned int      size;
  short             filler;
} sm_expr;

sm_expr *sm_new_expr(enum SM_EXPR_TYPE op1, sm_object *arg);
sm_expr *sm_new_expr_2(enum SM_EXPR_TYPE op1, sm_object *arg1, sm_object *arg2);
sm_expr *sm_new_expr_3(enum SM_EXPR_TYPE op1, sm_object *arg1, sm_object *arg2, sm_object *arg3);
sm_expr *sm_new_expr_n(enum SM_EXPR_TYPE op1, unsigned int size, unsigned int capacity);
sm_expr *sm_expr_append(sm_expr *expr, sm_object *arg);
unsigned int sm_prefix_sprint(sm_expr *self, char *buffer, bool fake);
unsigned int sm_infix_sprint(sm_expr *expr, char *buffer, bool fake);
sm_string   *sm_expr_to_string(sm_expr *expr);
unsigned int sm_expr_sprint(sm_expr *self, char *buffer, bool fake);
sm_expr     *sm_expr_set_arg(sm_expr *expr, unsigned int index, sm_object *num);
sm_object   *sm_expr_get_arg(sm_expr *expr, unsigned int index);
bool         sm_is_infix(enum SM_EXPR_TYPE op);
sm_object   *sm_expr_pop(sm_expr *sme);
sm_object   *sm_expr_pop_recycle(sm_expr *sme);
