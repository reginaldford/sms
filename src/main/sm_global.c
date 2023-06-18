// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "sms.h"

// Global object pointers
sm_heap   *sms_heap;
sm_heap   *sms_other_heap;
sm_symbol *sms_true;
sm_symbol *sms_false;

/* GLOBALS as functions
The following globals use functions f(x):
- If x is NULL, the static field is returned.
- If x is not NULL, the old field value is returned and x replaces the field.
This allows us to use a breakpoint or printf to detect reads/writes to the global.
*/

// tracking number of garbage collections
// instead of NULL, use 0
int sm_gc_count(int increase) {
  static int gc_count = 0;
  gc_count += increase;
  return gc_count;
}

// Replacement is ignored if less than 1.
// New capacity assumes this increment:
// new_cap = old_cap * growth_factor + 1;
// Lowest possible value for replacement setting is 1.
double sm_global_growth_factor(double replacement) {
  static double factor = 1.25;
  if (replacement >= 1.0) {
    int previous_factor = factor;
    factor              = replacement;
    return previous_factor;
  }
  return factor;
}

// the global space array sorted by size
sm_space_array *sm_global_space_array(sm_space_array *replacement) {
  static sm_space_array *spaces = NULL;
  if (replacement != NULL) {
    sm_space_array *temp = spaces;
    spaces               = replacement;
    return temp;
  }
  return spaces;
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

// Return the name of this type.
// Must be synchronized with enum sm_object_type
char *sm_global_type_name(unsigned short int which) {
  static char *response[] = {"num",   "xpr", "prm",   "str", "sym", "cx",  "node", "ptr",    "meta",
                             "space", "fun", "param", "lcl", "l",   "err", "self", "return", "?"};
  return response[which];
}

// Return the length of the name of this type.
// Must be synchronized with enum sm_object_type
unsigned short int sm_global_type_name_len(unsigned short int which) {
  static unsigned short int response_len[] = {3, 3, 3, 3, 3, 3, 3, 4, 5, 3, 5, 3, 3, 1};
  return response_len[which];
}

// primitive_names. read only
char *sm_global_fn_name(unsigned short int which) {
  const unsigned short int num_functions = sm_global_num_fns();
  // Should be syncronized with enum SM_EXPR_TYPE
  static char *response[] = {
    "exit",         // SM_EXIT_EXPR
    "help",         // SM_HELP_EXPR
    "clear",        // SM_CLEAR_EXPR
    "self",         // SM_SELF_EXPR
    "ls",           // SM_LS_EXPR
    "cd",           // SM_CD_EXPR
    "pwd",          // SM_PWD_EXPR
    "let",          // SM_LET_EXPR
    "=",            // SM_ASSIGN_EXPR
    "=",            // SM_ASSIGN_DOT_EXPR
    "=",            // SM_ASSIGN_LOCAL_EXPR
    "=",            // SM_ASSIGN_INDEX_EXPR
    "=",            // SM_ASSIGN_ELEMENT_EXPR
    "rm",           // SM_RM_EXPR
    ".",            // SM_DOT_EXPR
    "+",            // SM_PLUS_EXPR
    "-",            // SM_MINUS_EXPR
    "*",            // SM_TIMES_EXPR
    "/",            // SM_DIVIDE_EXPR
    "^",            // SM_POW_EXPR
    "sin",          // SM_SIN_EXPR
    "cos",          // SM_COS_EXPR
    "tan",          // SM_TAN_EXPR
    "asin",         // SM_ASIN_EXPR
    "acos",         // SM_ACOS_EXPR
    "atan",         // SM_ATAN_EXPR
    "sinh",         // SM_SINH_EXPR
    "cosh",         // SM_COSH_EXPR
    "tanh",         // SM_TANH_EXPR
    "asinh",        // SM_ASINH_EXPR
    "acosh",        // SM_ACOSH_EXPR
    "atanh",        // SM_ATANH_EXPR
    "csc",          // SM_CSC_EXPR
    "sec",          // SM_SEC_EXPR
    "cot",          // SM_COT_EXPR
    "acsc",         // SM_ACSC_EXPR
    "asec",         // SM_ASEC_EXPR
    "acot",         // SM_ACOT_EXPR
    "csch",         // SM_CSCH_EXPR
    "sech",         // SM_SECH_EXPR
    "coth",         // SM_COTH_EXPR
    "acsch",        // SM_ACSCH_EXPR
    "asech",        // SM_ASECH_EXPR
    "acoth",        // SM_ACOTH_EXPR
    "ln",           // SM_LN_EXPR
    "log",          // SM_LOG_EXPR
    "exp",          // SM_EXP_EXPR
    "sqrt",         // SM_SQRT_EXPR
    "abs",          // SM_ABS_EXPR
    "diff",         // SM_DIFF_EXPR
    "simp",         // SM_SIMP_EXPR
    "int",          // SM_INT_EXPR
    "if",           // SM_IF_EXPR
    "if",           // SM_IF_ELSE_EXPR
    "map",          // SM_MAP_EXPR
    "reduce",       // SM_REDUCE_EXPR
    "parent",       // SM_PARENT_EXPR
    "while",        // SM_WHILE_EXPR
    "doWhile",      // SM_DO_WHILE_EXPR
    "return",       // SM_RETURN_EXPR
    "eval",         // SM_EVAL_EXPR
    "cxEval",       // SM_CX_EVAL_EXPR
    "evalFast",     // SM_EVAL_FAST_EXPR
    "cxEvalFastIn", // SM_EVAL_FAST_IN_EXPR
    "fails",        // SM_FAILS_EXPR
    "parse",        // SM_PARSE_EXPR
    "meta",         // SM_RUNTIME_META_EXPR
    "==",           // SM_EQEQ_EXPR
    "->",           // SM_ASSOCIATE_EXPR
    "is",           // SM_IS_EXPR
    "<",            // SM_LT_EXPR
    ">",            // SM_GT_EXPR
    "<=",           // SM_LT_EQ_EXPR
    ">=",           // SM_GT_EQ_EXPR
    "",             // SM_INDEX_EXPR
    "",             // SM_BLOCK_EXPR
    "",             // SM_ARRAY_EXPR
    "toStr",        // SM_TO_STRING_EXPR
    "put",          // SM_PUT_EXPR
    "putLn",        // SM_PUTLN_EXPR
    "input",        // SM_INPUT_EXPR
    "cx",           // SM_NEW_CX_EXPR
    "cxDot",        // SM_CX_DOT_EXPR
    "cxLet",        // SM_CX_LET_EXPR
    "cxGet",        // SM_CX_GET_EXPR
    "cxHas",        // SM_CX_HAS_EXPR
    "cxGetFar",     // SM_CX_GET_FAR_EXPR
    "cxHasFar",     // SM_CX_HAS_FAR_EXPR
    "cxContaining", // SM_CX_CONTAINING_EXPR
    "cxSet",        // SM_CX_SET_EXPR
    "cxRm",         // SM_CX_RM_EXPR
    "cxClear",      // SM_CX_CLEAR_EXPR
    "cxKeys",       // SM_CX_KEYS_EXPR
    "cxValues",     // SM_CX_VALUES_EXPR
    "cxSize",       // SM_CX_SIZE_EXPR
    "cxImport",     // SM_CX_IMPORT_EXPR
    "cxMap",        // SM_CX_MAP_EXPR
    "fileParse",    // SM_FILE_PARSE_EXPR
    "fileRead",     // SM_FILE_READ_EXPR
    "fileRun",      // SM_FILE_RUN_EXPR
    "filePart",     // SM_FILE_PART_EXPR
    "fileExists",   // SM_FILE_EXISTS_EXPR
    "fileStat",     // SM_FILE_STAT_EXPR
    "fileToBlk",    // SM_FILE_TOBLK_EXPR
    "fileCp",       // SM_FILE_CP_EXPR
    "fileMv",       // SM_FILE_MV_EXPR
    "fileRm",       // SM_FILE_RM_EXPR
    "fileWrite",    // SM_FILE_WRITE_EXPR
    "fileApplend",  // SM_FILE_APPEND_EXPR
    "csvToArr",     // SM_CSV_TOARR_EXPR
    "arrToCsv",     // SM_ARR_TOCSV_EXPR
    "arr",          // SM_NEW_ARR_EXPR
    "cat",          // SM_ARR_CAT_EXPR
    "repeat",       // SM_ARR_REPEAT_EXPR
    "part",         // SM_PART_EXPR
    "size",         // SM_SIZE_EXPR
    "random",       // SM_RANDOM_EXPR
    "seed",         // SM_SEED_EXPR
    "round",        // SM_ROUND_EXPR
    "not",          // SM_NOT_EXPR
    "or",           // SM_OR_EXPR
    "and",          // SM_AND_EXPR
    "xor",          // SM_XOR_EXPR
    "strSize",      // SM_STR_SIZE_EXPR
    "strGet",       // SM_STR_GET_EXPR
    "strSet",       // SM_STR_SET_EXPR
    "strMap",       // SM_STR_MAP_EXPR
    "strFind",      // SM_STR_FIND_EXPR
    "strFindr",     // SM_STR_FINDR_EXPR
    "strCat",       // SM_STR_CAT_EXPR
    "strSplit",     // SM_STR_SPLIT_EXPR
    "strPart",      // SM_STR_PART_EXPR
    "strEscape",    // SM_STR_ESCAPE_EXPR
    "strUnescape",  // SM_STR_UNESCAPE_EXPR
    "strToNum",     // SM_STR_TONUMS_EXPR
    "strCmp",       // SM_STR_CMP_EXPR
    "strRepeat",    // SM_STR_REPEAT_EXPR
    "strToMem",     // SM_STR_TOBLK_EXPR
    "str",          // SM_NEW_STR_EXPR
    "toStr",        // SM_TO_STR_EXPR
    "strFmt",       // SM_TO_STRFMT_EXPR
    "newBlk",       // SM_NEW_BLK_EXPR
    "blkMap",       // SM_BLK_MAP_EXPR
    "blkUnite",     // SM_BLK_UNITE_EXPR
    "blkPart",      // SM_BLK_PART_EXPR
    "blkToNums",    // SM_BLK_TONUMS_EXPR
    "numsToBlk",    // SM_NUMS_TOBLK_EXPR
    "blkToFile",    // SM_BLK_TOFILE_EXPR
    "blkPrint",     // SM_BLK_PRINT_EXPR
    "blkSet",       // SM_BLK_SET_EXPR
    "blkGet",       // SM_BLK_GET_EXPR
    "blkToStr",     // SM_BLK_TOSTR_EXPR
    "blkSize",      // SM_BLK_SIZE_EXPR
    "",             // SM_FUN_CALL_EXPR
    "",             // SM_PARAM_LIST_EXPR
    "siblings",     // SM_SIBLINGS_EXPR
    "prim",         // SM_PRIM_EXPR
    "dateStr",      // SM_DATE_STR_EXPR
    "date",         // SM_DATE_EXPR
    "time",         // SM_TIME_EXPR
    "sleep",        // SM_SLEEP_EXPR
    "osFork",       // SM_FORK_EXPR
    "osWait",       // SM_WAIT_EXPR
    "osExec",       // SM_EXEC_EXPR
    "osGetEnv",     // SM_OS_GETENV_EXPR
    "osSetEnv",     // SM_OS_SETENV_EXPR
    "_gc",          // SM_GC_EXPR
    "?",            // SM_UNKNOWN_EXP
  };
  if (which >= num_functions)
    return response[num_functions];
  return response[which];
}

// corresponding string length of the string that would come from the sm_global_fn_name(which)
unsigned int sm_global_fn_name_len(unsigned short int which) {
  static long unsigned int response_len[] = {
    4, 4, 5, 4, 2,  2, 3, 3,  1, 1, 1, 1, 1, 2, 1, 1, 1, 1,  1, 1, 3, 3,  3, 4, 4,  4, 4, 4,
    4, 5, 5, 5, 3,  3, 3, 4,  4, 4, 4, 4, 4, 5, 5, 5, 2, 3,  3, 4, 3, 4,  4, 3, 2,  2, 3, 6,
    6, 5, 7, 6, 4,  6, 8, 12, 5, 5, 4, 2, 2, 2, 1, 1, 2, 2,  0, 0, 0, 5,  3, 5, 5,  2, 5, 5,
    5, 5, 8, 8, 12, 5, 4, 7,  6, 8, 6, 8, 5, 9, 8, 7, 8, 10, 8, 9, 6, 6,  6, 9, 11, 8, 8, 3,
    3, 6, 4, 4, 6,  4, 5, 3,  2, 3, 3, 7, 6, 6, 6, 7, 8, 6,  8, 7, 9, 11, 8, 6, 9,  8, 3, 5,
    6, 6, 6, 8, 7,  9, 9, 9,  8, 6, 6, 8, 7, 0, 0, 8, 4, 7,  4, 4, 5, 6,  6, 6, 8,  8, 3, 1};
  if (which >= sm_global_num_fns())
    return 1; // "?"
  return response_len[which];
}

unsigned int sm_global_num_fns() {
  static const unsigned short int num_fns = 161;
  return num_fns;
}

// This makes the bison parser globally accessible
sm_object *sm_global_parser_output(sm_object *replacement) {
  static sm_object *parser_output;
  if (replacement != NULL) {
    sm_object *temp = parser_output;
    parser_output   = replacement;
    return temp;
  } else
    return parser_output;
}

// Options from the command line arguments
sm_env *sm_global_environment(sm_env *replacement) {
  static sm_env *options;
  if (replacement != NULL) {
    sm_env *temp = options;
    options      = replacement;
    return temp;
  } else
    return options;
}
