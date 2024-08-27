// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

// Denotes the type of expression
// Must be in the same order as sm_global_fn_name

enum SM_EXPR_TYPE {
  SM_VERSION_EXPR,
  SM_EXIT_EXPR,
  SM_HELP_EXPR,
  SM_CLEAR_EXPR,
  SM_SELF_EXPR,
  SM_LS_EXPR,
  SM_CD_EXPR,
  SM_PWD_EXPR,
  SM_LET_EXPR,
  SM_ASSIGN_EXPR,
  SM_ASSIGN_DOT_EXPR,
  SM_ASSIGN_LOCAL_EXPR,
  SM_ASSIGN_INDEX_EXPR,
  SM_ASSIGN_ELEMENT_EXPR,
  SM_RM_EXPR,
  SM_DOT_EXPR,
  SM_PLUS_EXPR,
  SM_MINUS_EXPR,
  SM_TIMES_EXPR,
  SM_DIVIDE_EXPR,
  SM_POW_EXPR,
  SM_IPLUS_EXPR,
  SM_IMINUS_EXPR,
  SM_ITIMES_EXPR,
  SM_IDIVIDE_EXPR,
  SM_IPOW_EXPR,
  SM_IXOR_EXPR,
  SM_IAND_EXPR,
  SM_IOR_EXPR,
  SM_PLUSEQ_EXPR,
  SM_MINUSEQ_EXPR,
  SM_TIMESEQ_EXPR,
  SM_DIVIDEEQ_EXPR,
  SM_POWEREQ_EXPR,
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
  SM_INC_EXPR,
  SM_DEC_EXPR,
  SM_DIFF_EXPR,
  SM_SIMP_EXPR,
  SM_INT_EXPR,
  SM_IF_EXPR,
  SM_IF_ELSE_EXPR,
  SM_MAP_EXPR,
  SM_REDUCE_EXPR,
  SM_PARENT_EXPR,
  SM_WHILE_EXPR,
  SM_FOR_EXPR,
  SM_DO_WHILE_EXPR,
  SM_RETURN_EXPR,
  SM_EVAL_EXPR,
  SM_CX_EVAL_EXPR,
  SM_EVAL_FAST_EXPR,
  SM_EVAL_FAST_IN_EXPR,
  SM_FAILS_EXPR,
  SM_PARSE_EXPR,
  SM_RUNTIME_META_EXPR,
  SM_EQEQ_EXPR,
  SM_ASSOCIATE_EXPR,
  SM_IS_EXPR,
  SM_LT_EXPR,
  SM_GT_EXPR,
  SM_LT_EQ_EXPR,
  SM_GT_EQ_EXPR,
  SM_ISNAN_EXPR,
  SM_ISINF_EXPR,
  SM_INDEX_EXPR,
  SM_BLOCK_EXPR,
  SM_TUPLE_EXPR,
  SM_PUT_EXPR,
  SM_PUTLN_EXPR,
  SM_ARGS_EXPR,
  SM_INPUT_EXPR,
  SM_NEW_CX_EXPR,
  SM_CX_SETPARENT_EXPR,
  SM_CX_LET_EXPR,
  SM_CX_GET_EXPR,
  SM_CX_HAS_EXPR,
  SM_CX_GET_FAR_EXPR,
  SM_CX_HAS_FAR_EXPR,
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
  SM_FILE_CP_EXPR,
  SM_FILE_MV_EXPR,
  SM_FILE_RM_EXPR,
  SM_FILE_WRITE_EXPR,
  SM_FILE_WRITETGA_EXPR,
  SM_FILE_APPEND_EXPR,
  SM_ZEROS_EXPR,
  SM_PART_EXPR,
  SM_SIZE_EXPR,
  SM_CAT_EXPR,
  SM_REPEAT_EXPR,
  SM_RANDOM_EXPR,
  SM_SEED_EXPR,
  SM_ROUND_EXPR,
  SM_FLOOR_EXPR,
  SM_CEIL_EXPR,
  SM_MOD_EXPR,
  SM_NOT_EXPR,
  SM_OR_EXPR,
  SM_AND_EXPR,
  SM_XOR_EXPR,
  SM_FN_XP_EXPR,
  SM_FN_SETXP_EXPR,
  SM_FN_PARAMS_EXPR,
  SM_FN_SETPARAMS_EXPR,
  SM_FN_PARENT_EXPR,
  SM_FN_SETPARENT_EXPR,
  SM_XP_OP_EXPR,
  SM_XP_SET_OP_EXPR,
  SM_XP_OP_SYM_EXPR,
  SM_STR_SIZE_EXPR,
  SM_STR_MUT_EXPR,
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
  SM_NEW_STR_EXPR,
  SM_TO_STRFMT_EXPR,
  SM_FUN_CALL_EXPR,
  SM_PARAM_LIST_EXPR,
  SM_SIBLINGS_EXPR,
  SM_PRIM_EXPR,
  SM_DATE_STR_EXPR,
  SM_DATE_EXPR,
  SM_TIME_EXPR,
  SM_SLEEP_EXPR,
  SM_FORK_EXPR,
  SM_WAIT_EXPR,
  SM_EXEC_EXPR,
  SM_EXECTOSTR_EXPR,
  SM_OS_GETENV_EXPR,
  SM_OS_SETENV_EXPR,
  SM_GC_EXPR,
  SM_ISERR_EXPR,
  SM_ERRTITLE_EXPR,
  SM_ERRMESSAGE_EXPR,
  SM_ERRSOURCE_EXPR,
  SM_ERRLINE_EXPR,
  SM_ERRNOTES_EXPR,
  SM_UNKNOWN_EXPR,
};

// Head of an expression
// Assumed to have pointers following the structure for each argument
// Size denotes the number of trailing pointers.
typedef struct sm_expr {
  int32_t           my_type;  // 2
  enum SM_EXPR_TYPE op;       // 4
  uint32_t          capacity; // 4
  uint32_t          size;     // 4
  // ! notes would be sm_cx* , but dependancy cycle prevents
  // We use NULL to signify no notes.
  void *notes; // 8
} sm_expr;

/// New Expression with no args
sm_expr *sm_new_expr_0(enum SM_EXPR_TYPE op1, void *notes);
/// New Expression with 1 arg
sm_expr *sm_new_expr(enum SM_EXPR_TYPE op1, sm_object *arg, void *notes);
/// New Expression with 2 args
sm_expr *sm_new_expr_2(enum SM_EXPR_TYPE op1, sm_object *arg1, sm_object *arg2, void *notes);
/// New Expression with 3 args
sm_expr *sm_new_expr_3(enum SM_EXPR_TYPE op1, sm_object *arg1, sm_object *arg2, sm_object *arg3,
                       void *notes);
/// New Expression with 4 args
sm_expr *sm_new_expr_4(enum SM_EXPR_TYPE op, sm_object *arg1, sm_object *arg2, sm_object *arg3,
                       sm_object *arg4, void *notes);
/// New Expression with size args
sm_expr   *sm_new_expr_n(enum SM_EXPR_TYPE op2, uint32_t size, uint32_t capacity, void *notes);
sm_expr   *sm_expr_append(sm_expr *expr, sm_object *arg);
uint32_t   sm_prefix_sprint(sm_expr *self, char *buffer, bool fake);
uint32_t   sm_infix_sprint(sm_expr *expr, char *buffer, bool fake);
sm_string *sm_expr_to_string(sm_expr *expr);
uint32_t   sm_expr_sprint(sm_expr *self, char *buffer, bool fake);
sm_expr   *sm_expr_set_arg(sm_expr *expr, uint32_t index, sm_object *num);
sm_object *sm_expr_get_arg(sm_expr *expr, uint32_t index);
bool       sm_is_infix(enum SM_EXPR_TYPE op);
sm_object *sm_expr_pop(sm_expr *sme);
