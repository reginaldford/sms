// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "sms.h"

// Global object pointers
void      **memory_marker1;
void      **memory_marker2;
sm_heap_set sms_all_heaps;
sm_heap    *sms_heap;
sm_heap    *sms_other_heap;
sm_symbol  *sms_true;
sm_symbol  *sms_false;
// Symbols including the 2 above live in a symbol heap
sm_heap *sms_symbol_heap;
// Symbol names are in alpabetical order, correlating with the symbol
sm_heap   *sms_symbol_name_heap;
uint32_t   sms_num_symbols;
bool       evaluating;
sm_object *return_obj;
// 2 virtual machine  stacks
sm_stack2 *sms_stack;
sm_stack2 *sms_cx_stack;
sm_stack2 *sms_sf;
// function pointers for numbers
// f64, i64, ui64, ui8


sm_object *(*sm_add_functions[])(sm_object *o1, sm_object *o2) = {
  &sm_add_ui8_and_ui8,  &sm_add_ui8_and_ui64,  &sm_add_ui8_and_i64,  &sm_add_ui8_and_f64,
  &sm_add_ui64_and_ui8, &sm_add_ui64_and_ui64, &sm_add_ui64_and_i64, &sm_add_ui64_and_f64,
  &sm_add_i64_and_ui8,  &sm_add_i64_and_ui64,  &sm_add_i64_and_i64,  &sm_add_i64_and_f64,
  &sm_add_f64_and_ui8,  &sm_add_f64_and_ui64,  &sm_add_f64_and_i64,  &sm_add_f64_and_f64};

sm_object *(*sm_minus_functions[])(sm_object *o1, sm_object *o2) = {
  &sm_minus_ui8_and_ui8,  &sm_minus_ui8_and_ui64,  &sm_minus_ui8_and_i64,  &sm_minus_ui8_and_f64,
  &sm_minus_ui64_and_ui8, &sm_minus_ui64_and_ui64, &sm_minus_ui64_and_i64, &sm_minus_ui64_and_f64,
  &sm_minus_i64_and_ui8,  &sm_minus_i64_and_ui64,  &sm_minus_i64_and_i64,  &sm_minus_i64_and_f64,
  &sm_minus_f64_and_ui8,  &sm_minus_f64_and_ui64,  &sm_minus_f64_and_i64,  &sm_minus_f64_and_f64};

sm_object *(*sm_times_functions[])(sm_object *o1, sm_object *o2) = {
  &sm_times_ui8_and_ui8,  &sm_times_ui8_and_ui64,  &sm_times_ui8_and_i64,  &sm_times_ui8_and_f64,
  &sm_times_ui64_and_ui8, &sm_times_ui64_and_ui64, &sm_times_ui64_and_i64, &sm_times_ui64_and_f64,
  &sm_times_i64_and_ui8,  &sm_times_i64_and_ui64,  &sm_times_i64_and_i64,  &sm_times_i64_and_f64,
  &sm_times_f64_and_ui8,  &sm_times_f64_and_ui64,  &sm_times_f64_and_i64,  &sm_times_f64_and_f64};

sm_object *(*sm_divide_functions[])(sm_object *o1, sm_object *o2) = {
  &sm_divide_ui8_and_ui8,  &sm_divide_ui8_and_ui64, &sm_divide_ui8_and_i64,
  &sm_divide_ui8_and_f64,  &sm_divide_ui64_and_ui8, &sm_divide_ui64_and_ui64,
  &sm_divide_ui64_and_i64, &sm_divide_ui64_and_f64, &sm_divide_i64_and_ui8,
  &sm_divide_i64_and_ui64, &sm_divide_i64_and_i64,  &sm_divide_i64_and_f64,
  &sm_divide_f64_and_ui8,  &sm_divide_f64_and_ui64, &sm_divide_f64_and_i64,
  &sm_divide_f64_and_f64};


sm_object *(*sm_gt_functions[])(sm_object *o1, sm_object *o2) = {
  &sm_gt_ui8_and_ui8,  &sm_gt_ui8_and_ui64,  &sm_gt_ui8_and_i64,  &sm_gt_ui8_and_f64,
  &sm_gt_ui64_and_ui8, &sm_gt_ui64_and_ui64, &sm_gt_ui64_and_i64, &sm_gt_ui64_and_f64,
  &sm_gt_i64_and_ui8,  &sm_gt_i64_and_ui64,  &sm_gt_i64_and_i64,  &sm_gt_i64_and_f64,
  &sm_gt_f64_and_ui8,  &sm_gt_f64_and_ui64,  &sm_gt_f64_and_i64,  &sm_gt_f64_and_f64};

sm_object *(*sm_lt_functions[])(sm_object *o1, sm_object *o2) = {
  &sm_lt_ui8_and_ui8,  &sm_lt_ui8_and_ui64,  &sm_lt_ui8_and_i64,  &sm_lt_ui8_and_f64,
  &sm_lt_ui64_and_ui8, &sm_lt_ui64_and_ui64, &sm_lt_ui64_and_i64, &sm_lt_ui64_and_f64,
  &sm_lt_i64_and_ui8,  &sm_lt_i64_and_ui64,  &sm_lt_i64_and_i64,  &sm_lt_i64_and_f64,
  &sm_lt_f64_and_ui8,  &sm_lt_f64_and_ui64,  &sm_lt_f64_and_i64,  &sm_lt_f64_and_f64};

sm_object *(*sm_pow_functions[])(sm_object *o1, sm_object *o2) = {
  &sm_pow_ui8_and_ui8,  &sm_pow_ui8_and_ui64,  &sm_pow_ui8_and_i64,  &sm_pow_ui8_and_f64,
  &sm_pow_ui64_and_ui8, &sm_pow_ui64_and_ui64, &sm_pow_ui64_and_i64, &sm_pow_ui64_and_f64,
  &sm_pow_i64_and_ui8,  &sm_pow_i64_and_ui64,  &sm_pow_i64_and_i64,  &sm_pow_i64_and_f64,
  &sm_pow_f64_and_ui8,  &sm_pow_f64_and_ui64,  &sm_pow_f64_and_i64,  &sm_pow_f64_and_f64};

sm_object *(*sm_gteq_functions[])(sm_object *o1, sm_object *o2) = {
  &sm_gteq_ui8_and_ui8,  &sm_gteq_ui8_and_ui64,  &sm_gteq_ui8_and_i64,  &sm_gteq_ui8_and_f64,
  &sm_gteq_ui64_and_ui8, &sm_gteq_ui64_and_ui64, &sm_gteq_ui64_and_i64, &sm_gteq_ui64_and_f64,
  &sm_gteq_i64_and_ui8,  &sm_gteq_i64_and_ui64,  &sm_gteq_i64_and_i64,  &sm_gteq_i64_and_f64,
  &sm_gteq_f64_and_ui8,  &sm_gteq_f64_and_ui64,  &sm_gteq_f64_and_i64,  &sm_gteq_f64_and_f64};

/* GLOBALS as functions
 The following globals use functions f(x):
  - If x is NULL, the static field is returned.
  - If x is not NULL, the old field value is returned and x replaces the field.
 This allows us to use a breakpoint or printf to detect reads/writes to the global.
*/

// Tracking number of garbage collections
int sm_gc_count(int increase) {
  static int gc_count = 0;
  gc_count += increase;
  return gc_count;
}

// New capacity of tuples that need to grow:
// Replacement is ignored if less than 1.
// new_capacity = old_capacity * growth_factor + 1;
double sm_global_growth_factor(double replacement) {
  static double factor = 1.5;
  if (replacement >= 1.0) {
    int previous_factor = factor;
    factor              = replacement;
    return previous_factor;
  }
  return factor;
}

// the global lexical stack
sm_stack *sm_global_lex_stack(sm_stack *replacement) {
  static sm_stack *lex_stack = NULL;
  if (replacement != NULL) {
    sm_stack *temp = lex_stack;
    lex_stack      = replacement;
    return temp;
  }
  return lex_stack;
}

bool sm_global_fn_hidden(uint32_t which) {
  switch (which) {
  case SM_FN_PARAMS_EXPR:
  case SM_PARAM_LIST_EXPR:
  case SM_BLOCK_EXPR:
    return true;
    break;
  }
  return false;
}

// Primitive_names
char *sm_global_fn_name(uint32_t which) {
  const uint32_t num_functions = sm_global_num_fns();
  // Should be syncronized with enum SM_EXPR_TYPE
  static char *response[] = {
    "_version",      // SM_VERSION_EXPR
    "exit",          // SM_EXIT_EXPR
    "help",          // SM_HELP_EXPR
    "clear",         // SM_CLEAR_EXPR
    "self",          // SM_SELF_EXPR
    "ls",            // SM_LS_EXPR
    "cd",            // SM_CD_EXPR
    "pwd",           // SM_PWD_EXPR
    "let",           // SM_LET_EXPR
    "f64",           // SM_NEW_F64_EXPR
    "i64",           // SM_NEW_I64_EXPR
    "ui64",          // SM_NEW_UI64_EXPR
    "ui8",           // SM_NEW_UI8_EXPR
    "f64Repeat",     // SM_F64_REPEAT_EXPR
    "i64Repeat",     // SM_I64_REPEAT_EXPR
    "ui64Repeat",    // SM_UI64_REPEAT_EXPR
    "ui8Repeat",     // SM_UI8_REPEAT_EXPR
    "=",             // SM_ASSIGN_EXPR
    "=",             // SM_ASSIGN_DOT_EXPR
    "=",             // SM_ASSIGN_LOCAL_EXPR
    "=",             // SM_ASSIGN_INDEX_EXPR
    "=",             // SM_ASSIGN_ELEMENT_EXPR
    "rm",            // SM_RM_EXPR
    ".",             // SM_DOT_EXPR
    "+",             // SM_PLUS_EXPR
    "-",             // SM_MINUS_EXPR
    "*",             // SM_TIMES_EXPR
    "/",             // SM_DIVIDE_EXPR
    "**",            // SM_POW_EXPR
    "!+",            // SM_IPLUS_EXPR
    "!-",            // SM_IMINUS_EXPR
    "!*",            // SM_ITIMES_EXPR
    "!/",            // SM_IDIVIDE_EXPR
    "!**",           // SM_IPOW_EXPR
    "^",             // SM_IXOR_EXPR
    "&",             // SM_IAND_EXPR
    "|",             // SM_IOR_EXPR
    "_~",            // SM_INOT_EXPR
    "_~=",           // SM_INOTEQ_EXPR
    "^=",            // SM_IXOREQ_EXPR
    "&=",            // SM_IANDEQ_EXPR
    "|=",            // SM_IOREQ_EXPR
    "+=",            // SM_PLUSEQ_EXPR
    "-=",            // SM_MINUSEQ_EXPR
    "*=",            // SM_TIMESEQ_EXPR
    "/=",            // SM_DIVIDEEQ_EXPR
    "**=",           // SM_POWEREQ_EXPR
    "!+=",           // SM_IPLUSEQ_EXPR
    "!-=",           // SM_IMINUSEQ_EXPR
    "!*=",           // SM_ITIMESEQ_EXPR
    "!/=",           // SM_IDIVIDEEQ_EXPR
    "!**=",          // SM_IPOWEREQ_EXPR
    "sin",           // SM_SIN_EXPR
    "cos",           // SM_COS_EXPR
    "tan",           // SM_TAN_EXPR
    "asin",          // SM_ASIN_EXPR
    "acos",          // SM_ACOS_EXPR
    "atan",          // SM_ATAN_EXPR
    "sinh",          // SM_SINH_EXPR
    "cosh",          // SM_COSH_EXPR
    "tanh",          // SM_TANH_EXPR
    "asinh",         // SM_ASINH_EXPR
    "acosh",         // SM_ACOSH_EXPR
    "atanh",         // SM_ATANH_EXPR
    "csc",           // SM_CSC_EXPR
    "sec",           // SM_SEC_EXPR
    "cot",           // SM_COT_EXPR
    "acsc",          // SM_ACSC_EXPR
    "asec",          // SM_ASEC_EXPR
    "acot",          // SM_ACOT_EXPR
    "csch",          // SM_CSCH_EXPR
    "sech",          // SM_SECH_EXPR
    "coth",          // SM_COTH_EXPR
    "acsch",         // SM_ACSCH_EXPR
    "asech",         // SM_ASECH_EXPR
    "acoth",         // SM_ACOTH_EXPR
    "ln",            // SM_LN_EXPR
    "log",           // SM_LOG_EXPR
    "exp",           // SM_EXP_EXPR
    "sqrt",          // SM_SQRT_EXPR
    "abs",           // SM_ABS_EXPR
    "++",            // SM_INC_EXPR
    "--",            // SM_DEC_EXPR
    "++",            // SM_PREINC_EXPR
    "--",            // SM_PREDEC_EXPR
    "!++",           // SM_IINC_EXPR
    "!--",           // SM_IDEC_EXPR
    "!++",           // SM_PREIINC_EXPR
    "!--",           // SM_PREIDEC_EXPR
    "diff",          // SM_DIFF_EXPR
    "simp",          // SM_SIMP_EXPR
    "int",           // SM_INT_EXPR
    "if",            // SM_IF_EXPR
    "if",            // SM_IF_ELSE_EXPR
    "map",           // SM_MAP_EXPR
    "reduce",        // SM_REDUCE_EXPR
    "parent",        // SM_PARENT_EXPR
    "while",         // SM_WHILE_EXPR
    "for",           // SM_FOR_EXPR
    "forIn",         // SM_FOR_IN_EXPR
    "forInWhere",    // SM_FOR_IN_WHERE_EXPR
    "doWhile",       // SM_DO_WHILE_EXPR
    "return",        // SM_RETURN_EXPR
    "eval",          // SM_EVAL_EXPR
    "cxEval",        // SM_CX_EVAL_EXPR
    "evalFast",      // SM_EVAL_FAST_EXPR
    "cxEvalFastIn",  // SM_EVAL_FAST_IN_EXPR
    "fails",         // SM_FAILS_EXPR
    "parse",         // SM_PARSE_EXPR
    "meta",          // SM_RUNTIME_META_EXPR
    "==",            // SM_EQEQ_EXPR
    "->",            // SM_ASSOCIATE_EXPR
    "is",            // SM_IS_EXPR
    "<",             // SM_LT_EXPR
    ">",             // SM_GT_EXPR
    "<=",            // SM_LT_EQ_EXPR
    ">=",            // SM_GT_EQ_EXPR
    "isNan",         // SM_ISNAN_EXPR
    "isInf",         // SM_ISINF_EXPR
    "index",         // SM_INDEX_EXPR
    "block",         // SM_BLOCK_EXPR
    "tuple",         // SM_TUPLE_EXPR
    "put",           // SM_PUT_EXPR
    "putLn",         // SM_PUTLN_EXPR
    "args",          // SM_ARGS_EXPR
    "input",         // SM_INPUT_EXPR
    "cx",            // SM_NEW_CX_EXPR
    "cxSetParent",   // SM_SETPARENT_EXPR,
    "cxLet",         // SM_CX_LET_EXPR
    "cxGet",         // SM_CX_GET_EXPR
    "cxHas",         // SM_CX_HAS_EXPR
    "cxGetFar",      // SM_CX_GET_FAR_EXPR
    "cxHasFar",      // SM_CX_HAS_FAR_EXPR
    "cxContaining",  // SM_CX_CONTAINING_EXPR
    "cxSet",         // SM_CX_SET_EXPR
    "cxRm",          // SM_CX_RM_EXPR
    "cxClear",       // SM_CX_CLEAR_EXPR
    "cxKeys",        // SM_CX_KEYS_EXPR
    "cxValues",      // SM_CX_VALUES_EXPR
    "cxSize",        // SM_CX_SIZE_EXPR
    "cxImport",      // SM_CX_IMPORT_EXPR
    "cxMap",         // SM_CX_MAP_EXPR
    "fileParse",     // SM_FILE_PARSE_EXPR
    "fileReadStr",   // SM_FILE_READSTR_EXPR
    "fileRead",      // SM_FILE_READ
    "fileRun",       // SM_FILE_RUN_EXPR
    "filePart",      // SM_FILE_PART_EXPR
    "fileExists",    // SM_FILE_EXISTS_EXPR
    "fileStat",      // SM_FILE_STAT_EXPR
    "fileCp",        // SM_FILE_CP_EXPR
    "fileMv",        // SM_FILE_MV_EXPR
    "fileRm",        // SM_FILE_RM_EXPR
    "fileWriteStr",  // SM_FILE_WRITESTR_EXPR
    "fileWriteArr",  // SM_FILE_WRITEARR_EXPR
    "fileAppend",    // SM_FILE_APPEND_EXPR
    "zeros",         // SM_ZEROS_EXPR
    "part",          // SM_PART_EXPR
    "size",          // SM_SIZE_EXPR
    "cat",           // SM_CAT_EXPR
    "repeat",        // SM_REPEAT_EXPR
    "random",        // SM_RANDOM_EXPR
    "seed",          // SM_SEED_EXPR
    "round",         // SM_ROUND_EXPR
    "floor",         // SM_FLOOR_EXPR
    "ceil",          // SM_CEIL_EXPR
    "mod",           // SM_MOD_EXPR
    "~",             // SM_NOT_EXPR
    "||",            // SM_OR_EXPR
    "&&",            // SM_AND_EXPR
    "^^",            // SM_XOR_EXPR
    "~=",            // SM_NOTEQ_EXPR
    "||=",           // SM_OREQ_EXPR
    "&&=",           // SM_ANDEQ_EXPR
    "^^=",           // SM_XOREQ_EXPR
    "fnXp",          // SM_FNXP_EXPR
    "fnSetXp",       // SM_FNSETXP_EXPR
    "fnParams",      // SM_FN_PARAMS_EXPR
    "fnSetParams",   // SM_FN_SETPARAMS_EXPR
    "fnParent",      // SM_FN_PARENT_EXPR
    "fnSetParent",   // SM_FN_SETPARENT_EXPR
    "xpOp",          // SM_XP_OP
    "xpSetOp",       // SM_XP_SET_OP_EXPR
    "xpOpSym",       // SM_XP_OP_SYM
    "strSize",       // SM_STR_SIZE_EXPR
    "strMut",        // SM_STR_MUT_EXPR
    "strGet",        // SM_STR_GET_EXPR
    "strSet",        // SM_STR_SET_EXPR
    "strMap",        // SM_STR_MAP_EXPR
    "strFind",       // SM_STR_FIND_EXPR
    "strFindR",      // SM_STR_FINDR_EXPR
    "str+",          // SM_STR_CAT_EXPR
    "strSplit",      // SM_STR_SPLIT_EXPR
    "strPart",       // SM_STR_PART_EXPR
    "strEscape",     // SM_STR_ESCAPE_EXPR
    "strUnescape",   // SM_STR_UNESCAPE_EXPR
    "strToNum",      // SM_STR_TONUMS_EXPR
    "strCmp",        // SM_STR_CMP_EXPR
    "strRepeat",     // SM_STR_REPEAT_EXPR
    "str",           // SM_NEW_STR_EXPR
    "strFmt",        // SM_TO_STRFMT_EXPR
    "functionCall",  // SM_FUN_CALL_EXPR
    "parameterList", // SM_PARAM_LIST_EXPR
    "siblings",      // SM_SIBLINGS_EXPR
    "primitive",     // SM_PRIM_EXPR
    "dateStr",       // SM_DATE_STR_EXPR
    "date",          // SM_DATE_EXPR
    "time",          // SM_TIME_EXPR
    "sleep",         // SM_SLEEP_EXPR
    "osFork",        // SM_FORK_EXPR
    "osWait",        // SM_WAIT_EXPR
    "osExec",        // SM_EXEC_EXPR
    "osExecToStr",   // SM_EXECTOSTR_EXPR
    "osGetEnv",      // SM_OS_GETENV_EXPR
    "osSetEnv",      // SM_OS_SETENV_EXPR
    "_heapSave",     // SM_HEAPSAVE_EXPR
    "_imgSave",      // SM_IMAGESAVE_EXPR
    "_gc",           // SM_GC_EXPR
    "isErr",         // SM_ISERR_EXPR
    "errTitle",      // SM_ERRTITLE_EXPR
    "errMessage",    // SM_ERRMESSAGE_EXPR
    "errSource",     // SM_ERRSOURCE_EXPR
    "errLine",       // SM_ERRLINE_EXPR
    "errNotes",      // SM_ERRNOTES_EXPR
    "import",        // SM_IMPORT_EXPR
    "soLoad",        // SM_SOLOAD_EXPR
    "soFun",         // SM_SOFUN_EXPR
    "?",             // SM_UNKNOWN_EXP
  };
  if (which >= num_functions)
    return response[num_functions];
  return response[which];
}

// Corresponding string length of the string that would come from the sm_global_fn_name(which)
uint32_t sm_global_fn_name_len(uint32_t which) {
  static uint16_t response_len[] = {
    8, 4, 4,  5,  4, 2, 2, 3, 3, 3, 3, 4,  3,  9,  9, 10, 9, 1,  1,  1,  1, 1,  2,  1,  1,  1,
    1, 1, 2,  2,  2, 2, 2, 3, 1, 1, 1, 2,  3,  2,  2, 2,  2, 2,  2,  2,  3, 3,  3,  3,  3,  4,
    3, 3, 3,  4,  4, 4, 4, 4, 4, 5, 5, 5,  3,  3,  3, 4,  4, 4,  4,  4,  4, 5,  5,  5,  2,  3,
    3, 4, 3,  2,  2, 2, 2, 3, 3, 3, 3, 4,  4,  3,  2, 2,  3, 6,  6,  5,  3, 5,  10, 7,  6,  4,
    6, 8, 12, 5,  5, 4, 2, 2, 2, 1, 1, 2,  2,  5,  5, 5,  5, 5,  3,  5,  4, 5,  2,  11, 5,  5,
    5, 8, 8,  12, 5, 4, 7, 6, 8, 6, 8, 5,  9,  11, 8, 7,  8, 10, 8,  6,  6, 6,  12, 12, 10, 5,
    4, 4, 3,  6,  6, 4, 5, 5, 4, 3, 1, 2,  2,  2,  2, 3,  3, 3,  4,  7,  8, 11, 8,  11, 4,  7,
    7, 7, 6,  6,  6, 6, 7, 8, 4, 8, 7, 9,  11, 8,  6, 9,  3, 6,  12, 13, 8, 9,  7,  4,  4,  5,
    6, 6, 6,  11, 8, 8, 9, 8, 3, 5, 8, 10, 9,  7,  8, 6,  6, 5,  1};


  if (which >= sm_global_num_fns())
    return 1; // "?"
  return response_len[which];
}

uint32_t sm_global_num_fns() {
  static const uint32_t num_fns = 223;
  return num_fns;
}

// Bison parser output
sm_object *sm_global_parser_output(sm_object *replacement) {
  static sm_object *parser_output;
  if (replacement != NULL) {
    sm_object *temp = parser_output;
    parser_output   = replacement;
    return temp;
  } else
    return parser_output;
}

// Environment structure
sm_env *sm_global_environment(sm_env *replacement) {
  static sm_env *environment;
  if (replacement != NULL) {
    sm_env *temp = environment;
    environment  = replacement;
    return temp;
  } else
    return environment;
}

// Read only sm_string for home directory, or NULL
sm_string *sm_global_home_directory() {
  static char home_dir[257];
  if (!*home_dir) {
    uid_t          uid = getuid();      // Get the user ID of the current process
    struct passwd *pw  = getpwuid(uid); // Get passwd struct for the user
    if (pw == NULL)
      return NULL; // Error
    sm_string *str = (sm_string *)home_dir;
    str->my_type   = SM_STRING_TYPE;
    str->size      = strlen(pw->pw_dir);
    sm_strncpy(&(str->content), pw->pw_dir, str->size);
  }
  return (sm_string *)home_dir;
}

// Read only sm_string for home directory, or NULL
sm_string *sm_global_hist_file() {
  static char hist_file_string_space[256];
  if (!*hist_file_string_space) {
    sm_env     env = *sm_global_environment(NULL);
    sm_string *str = (sm_string *)hist_file_string_space;
    str->my_type   = SM_STRING_TYPE;
    str->size      = env.history_file_len;
    sm_strncpy(&(str->content), env.history_file, env.history_file_len);
  }
  return (sm_string *)hist_file_string_space;
}

// global version string
sm_string *sms_global_version() {
  static char version_string_space[16 + sizeof(struct sm_string)];
  if (!*version_string_space) {
    sm_env     env = *sm_global_environment(NULL);
    sm_string *str = (sm_string *)version_string_space;
    str->my_type   = SM_STRING_TYPE;
    str->size      = env.version_len;
    sm_strncpy(&(str->content), env.version, env.version_len);
  }
  return (sm_string *)version_string_space;
}
