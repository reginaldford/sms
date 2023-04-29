// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "sms.h"

/* GLOBALS
Globals in this project use functions f(x):
- If x is NULL, the static field is returned.
- If x is not NULL, the old field value is returned and x replaces the field.
This allows us to use a breakpoint or printf to detect reads/writes to the global.
*/

// the global context
sm_cx *sm_globalcx(sm_cx *replacement) {
  static sm_cx *globalcx = NULL;
  if (replacement != NULL) {
    sm_cx *temp = globalcx;
    globalcx    = replacement;
    return temp;
  }
  return globalcx;
}

// Current heap. Objects here are live
sm_heap *sm_global_current_heap(sm_heap *replacement) {
  static sm_heap *current_heap = NULL;
  if (replacement != NULL) {
    sm_heap *temp = current_heap;
    current_heap  = replacement;
    return temp;
  }
  return current_heap;
}

// 'other' heap, also known as 'to' heap
sm_heap *sm_global_other_heap(sm_heap *replacement) {
  static sm_heap *other_heap = NULL;
  if (replacement != NULL) {
    sm_heap *temp = other_heap;
    other_heap    = replacement;
    return temp;
  }
  return other_heap;
}

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
  static char *response[] = {"num",  "xpr",   "prm", "str",   "sym", "cxt", "ptr",
                             "meta", "space", "fun", "param", "lcl", "err", "?"};
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
  static char *response[] = {"exit",
                             "help",
                             "clear",
                             "self",
                             "ls",
                             "cd",
                             "pwd",
                             "let",
                             "=",
                             "=",
                             "=",
                             "=",
                             "rm",
                             "dot",
                             "+",
                             "-",
                             "*",
                             "/",
                             "^",
                             "sin",
                             "cos",
                             "tan",
                             "asin",
                             "acos",
                             "atan",
                             "sinh",
                             "cosh",
                             "tanh",
                             "asinh",
                             "acosh",
                             "atanh",
                             "csc",
                             "sec",
                             "cot",
                             "acsc",
                             "asec",
                             "acot",
                             "csch",
                             "sech",
                             "coth",
                             "acsch",
                             "asech",
                             "acoth",
                             "ln",
                             "log",
                             "exp",
                             "sqrt",
                             "abs",
                             "diff",
                             "simp",
                             "int",
                             "if",
                             "if",
                             "map",
                             "parent",
                             "while",
                             "doWhile",
                             "eval",
                             "evalInCX",
                             "evalFast",
                             "evalFastInCX",
                             "error",
                             "parse",
                             "meta",
                             "==",
                             "<",
                             ">",
                             "<=",
                             ">=",
                             "",
                             "",
                             "",
                             "toStr",
                             "put",
                             "putln",
                             "input",
                             "fileParse",
                             "fileRead",
                             "fileRun",
                             "filePart",
                             "fileExists",
                             "fileStat",
                             "fileToBlk",
                             "fileCp",
                             "fileMv",
                             "fileRm",
                             "fileWrite",
                             "fileAppend",
                             "csvToArr",
                             "arrToCSV",
                             "newArr",
                             "arrCat",
                             "arrRpt",
                             "part",
                             "size",
                             "random",
                             "seed",
                             "round",
                             "not",
                             "or",
                             "xor",
                             "strSize",
                             "strGet",
                             "strSet",
                             "strMap",
                             "strFind",
                             "strFindR",
                             "str+",
                             "strSplit",
                             "strPart",
                             "strEscape",
                             "strUnescape",
                             "strToNums",
                             "strCmp",
                             "strRpt",
                             "strToBlk",
                             "newStr",
                             "toStr",
                             "toStrFmt",
                             "newBlk",
                             "blkMap",
                             "blkUnite",
                             "blkPart",
                             "blkToNums",
                             "numsToBlk",
                             "blkToFile",
                             "blkPrint",
                             "blkSet",
                             "blkGet",
                             "blkToStr",
                             "blkSize",
                             "",
                             "",
                             "siblings",
                             "prim",
                             "dateStr",
                             "date",
                             "time",
                             "sleep",
                             "?"};
  if (which >= num_functions) {
    return response[num_functions];
  }
  return response[which];
}

// corresponding string length of the string that would come from the sm_global_fn_name(which)
unsigned int sm_global_fn_name_len(unsigned short int which) {
  static long unsigned int response_len[] = {
    4, 4, 5, 4,  2,  2, 3, 3, 1, 1, 1, 1, 2, 3, 1, 1, 1, 1, 1, 3, 3, 3, 4, 4, 4,  4, 4, 4,
    5, 5, 5, 3,  3,  3, 4, 4, 4, 4, 4, 4, 5, 5, 5, 2, 3, 3, 4, 3, 4, 4, 3, 2, 2,  3, 6, 5,
    7, 4, 8, 8,  12, 5, 5, 4, 2, 1, 1, 2, 2, 0, 0, 0, 5, 3, 5, 5, 9, 8, 7, 8, 10, 8, 9, 6,
    6, 6, 9, 10, 8,  8, 6, 6, 6, 4, 4, 6, 4, 5, 3, 2, 3, 7, 6, 6, 6, 7, 8, 4, 8,  7, 9, 11,
    9, 6, 6, 8,  6,  5, 8, 6, 6, 8, 7, 9, 9, 9, 8, 6, 6, 8, 7, 0, 0, 8, 4, 7, 4,  4, 5, 1};
  if (which >= sm_global_num_fns()) {
    return 1; // "?"
  }
  return response_len[which];
}

unsigned int sm_global_num_fns() {
  static const unsigned short int num_fns = 140;
  return num_fns;
}

// list of parent objects
// stored outside of heap and used for gc
sm_expr *sm_global_parents(sm_expr *replacement) {
  const int       initial_capacity = 100;
  static sm_expr *parents          = NULL;
  if (replacement != NULL) {
    sm_expr *previous = parents;
    parents           = replacement;
    return previous;
  }
  if (parents == NULL) {
    sm_expr *parents  = (sm_expr *)malloc(sizeof(sm_expr) + sizeof(void *) * initial_capacity);
    parents->my_type  = SM_EXPR_TYPE;
    parents->op       = SM_SIBLINGS_EXPR;
    parents->size     = 0;
    parents->capacity = initial_capacity;
    return parents;
  }
  return parents;
}

// This makes the bison parser globally accessible
sm_object *sm_global_parser_output(sm_object *replacement) {
  static sm_object *parser_output;
  if (replacement != NULL) {
    sm_object *temp = parser_output;
    parser_output   = replacement;
    return temp;
  } else {
    return parser_output;
  }
}

// Options from the command line arguments
sm_env *sm_global_environment(sm_env *replacement) {
  static sm_env *options;
  if (replacement != NULL) {
    sm_env *temp = options;
    options      = replacement;
    return temp;
  } else {
    return options;
  }
}
