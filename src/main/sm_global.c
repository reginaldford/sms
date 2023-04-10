// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "sms.h"

/* GLOBALS
Globals in this project use functions f(x):
- If x is NULL, the static field is returned.
- If x is not NULL, the old field value is returned and x replaces the field.
This allows us to use a breakpoint or printf to detect reads/writes to the global.
*/

// the global context
sm_context *sm_global_context(sm_context *replacement) {
  static sm_context *global_context = NULL;
  if (replacement != NULL) {
    sm_context *temp = global_context;
    global_context   = replacement;
    return temp;
  }
  return global_context;
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

// primitive_names. read only
char *sm_global_fn_name(unsigned short int which) {
  const unsigned short int num_functions = sm_global_num_fns();
  // Should be syncronized with enum SM_EXPR_TYPE
  static char *response[] = {"exit","help","clear","self","ls","cd","pwd","let","=","rm",".","+","-","*","/","^","sin","cos","tan","asin","acos","atan","sinh","cosh","tanh","asinh","acosh","atanh","csc","sec","cot","acsc","asec","acot","csch","sech","coth","acsch","asech","acoth","ln","log","exp","sqrt","abs","diff","simp","int","if","if","map","parent","while","eval","eval_fast","parse","meta","==","<",">","<=",">=",";","","","to_str","print","input","file_parse","file_tostr","file_toblk","file_exists","file_date","file_cp","file_mv","file_rm","file_size","str_tofile","csv_toarr","arr_tocsv","new_arr","arr+","arr*","part","size","random","round","not","or","xor","str_size","str+","str_get","str_set","str_map","str_find","str_findr","str_split","str_part","str_unescape","str_escape","str_setchar","str_getchar","str_tonums","str_tofile","str_cmp","str_repeat","str_toblk","new_str","to_str","to_strfmt","new_blk","blk_map","blk_unite","blk_part","blk_tonums","nums_toblk","blk_tofile","blk_output","blk_set","blk_get","blk_tostr","blk_size","","","siblings","prim","date_str","date","time","sleep","?"};
  if (which >= num_functions) {
    return response[num_functions];
  }
  return response[which];
}

// corresponding string length of the string that would come from the sm_global_fn_name(which)
unsigned int sm_global_fn_name_len(unsigned short int which) {
  static long unsigned int response_len[] = {4,4,5,4,2,2,3,3,1,2,1,1,1,1,1,1,3,3,3,4,4,4,4,4,4,5,5,5,3,3,3,4,4,4,4,4,4,5,5,5,2,3,3,4,3,4,4,3,2,2,3,6,5,4,9,5,4,2,1,1,2,2,1,0,0,6,5,5,10,10,10,11,9,7,7,7,9,10,9,9,7,4,4,4,4,6,5,3,2,3,8,4,7,7,7,8,9,9,8,12,10,11,11,10,10,7,10,9,7,6,9,7,7,9,8,10,10,10,10,7,7,9,8,0,0,8,4,8,4,4,5,1};
  if (which >= sm_global_num_fns()) {
    return 1; // "?"
  }
  return response_len[which];
}

unsigned int sm_global_num_fns() {
  static const unsigned short int num_fns = 132;
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
sm_options *sm_global_options(sm_options *replacement) {
  static sm_options *options;
  if (replacement != NULL) {
    sm_options *temp = options;
    options          = replacement;
    return temp;
  } else {
    return options;
  }
}
