// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

// Denotes the type of expression
// Must be in the same order as sm_global_fn_name
enum SM_EXPR_TYPE {
  SM_EXIT_EXPR,
  SM_HELP_EXPR,
  SM_CLEAR_EXPR,
  SM_SELF_EXPR,
  SM_LS_EXPR,
  SM_CD_EXPR,
  SM_PWD_EXPR,
  SM_LET_EXPR,
  SM_ASSIGN_EXPR,
  SM_ASSIGN_LOCAL_EXPR,
  SM_ASSIGN_INDEX_EXPR,
  SM_ASSIGN_DOT_EXPR,
  SM_RM_EXPR,
  SM_DOT_EXPR,
  SM_PLUS_EXPR,
  SM_MINUS_EXPR,
  SM_TIMES_EXPR,
  SM_DIVIDE_EXPR,
  SM_POW_EXPR,
  SM_SIN_EXPR,
  SM_COS_EXPR,
  SM_TAN_EXPR,
  SM_ASIN_EXPR,
  SM_ACOS_EXPR,
  SM_ATAN_EXPR,
  SM_SINH_EXPR,
  SM_COSH_EXPR,
  SM_TANH_EXPR,
  SM_ASINH_EXPR,
  SM_ACOSH_EXPR,
  SM_ATANH_EXPR,
  SM_CSC_EXPR,
  SM_SEC_EXPR,
  SM_COT_EXPR,
  SM_ACSC_EXPR,
  SM_ASEC_EXPR,
  SM_ACOT_EXPR,
  SM_CSCH_EXPR,
  SM_SECH_EXPR,
  SM_COTH_EXPR,
  SM_ACSCH_EXPR,
  SM_ASECH_EXPR,
  SM_ACOTH_EXPR,
  SM_LN_EXPR,
  SM_LOG_EXPR,
  SM_EXP_EXPR,
  SM_SQRT_EXPR,
  SM_ABS_EXPR,
  SM_DIFF_EXPR,
  SM_SIMP_EXPR,
  SM_INT_EXPR,
  SM_IF_EXPR,
  SM_IF_ELSE_EXPR,
  SM_MAP_EXPR,
  SM_PARENT_EXPR,
  SM_WHILE_EXPR,
  SM_DO_WHILE_EXPR,
  SM_EVAL_EXPR,
  SM_EVAL_IN_CX_EXPR,
  SM_EVAL_FAST_EXPR,
  SM_EVAL_FAST_IN_CX_EXPR,
  SM_FAILS_EXPR,
  SM_PARSE_EXPR,
  SM_RUNTIME_META_EXPR,
  SM_EQEQ_EXPR,
  SM_LT_EXPR,
  SM_GT_EXPR,
  SM_LT_EQ_EXPR,
  SM_GT_EQ_EXPR,

  SM_INDEX_EXPR,
  SM_THEN_EXPR,
  SM_ARRAY_EXPR,

  SM_TO_STRING_EXPR,
  SM_PUT_EXPR,
  SM_PUTLN_EXPR,
  SM_INPUT_EXPR,

  SM_NEW_CX_EXPR,
  SM_CX_DOT_EXPR,
  SM_CX_LET_EXPR,
  SM_CX_CONTAINING_EXPR,
  SM_CX_SET_EXPR,
  SM_CX_RM_EXPR,
  SM_CX_CLEAR_EXPR,
  SM_CX_KEYS_EXPR,
  SM_CX_VALUES_EXPR,
  SM_CX_SIZE_EXPR,
  SM_CX_IMPORT_EXPR,
  SM_CX_MAP_EXPR,

  SM_FILE_PARSE_EXPR,
  SM_FILE_READ_EXPR,
  SM_FILE_RUN_EXPR,
  SM_FILE_PART_EXPR,
  SM_FILE_EXISTS_EXPR,
  SM_FILE_STAT_EXPR,
  SM_FILE_TOBLK_EXPR,
  SM_FILE_CP_EXPR,
  SM_FILE_MV_EXPR,
  SM_FILE_RM_EXPR,
  SM_FILE_WRITE_EXPR,
  SM_FILE_APPEND_EXPR,

  SM_CSV_TOARR_EXPR,
  SM_ARR_TOCSV_EXPR,

  SM_NEW_ARR_EXPR,
  SM_ARR_CAT_EXPR,
  SM_ARR_REPEAT_EXPR,
  SM_PART_EXPR,
  SM_SIZE_EXPR,

  SM_RANDOM_EXPR,
  SM_SEED_EXPR,
  SM_ROUND_EXPR,
  SM_NOT_EXPR,
  SM_OR_EXPR,
  SM_XOR_EXPR,

  SM_STR_SIZE_EXPR,
  SM_STR_GET_EXPR,
  SM_STR_SET_EXPR,
  SM_STR_MAP_EXPR,
  SM_STR_FIND_EXPR,
  SM_STR_FINDR_EXPR,
  SM_STR_CAT_EXPR,
  SM_STR_SPLIT_EXPR,
  SM_STR_PART_EXPR,
  SM_STR_ESCAPE_EXPR,
  SM_STR_UNESCAPE_EXPR,
  SM_STR_TONUMS_EXPR,
  SM_STR_CMP_EXPR,
  SM_STR_REPEAT_EXPR,
  SM_STR_TOBLK_EXPR,
  SM_NEW_STR_EXPR,
  SM_TO_STR_EXPR,
  SM_TO_STRFMT_EXPR,

  SM_NEW_BLK_EXPR,
  SM_BLK_MAP_EXPR,
  SM_BLK_UNITE_EXPR,
  SM_BLK_PART_EXPR,
  SM_BLK_TONUMS_EXPR,
  SM_NUMS_TOBLK_EXPR,
  SM_BLK_TOFILE_EXPR,
  SM_BLK_PRINT_EXPR,
  SM_BLK_SET_EXPR,
  SM_BLK_GET_EXPR,
  SM_BLK_TOSTR_EXPR,
  SM_BLK_SIZE_EXPR,

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
