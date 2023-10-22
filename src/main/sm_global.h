// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#ifndef SMS_GLOBALS
#define SMS_GLOBALS

/// The currently used memory heap
extern sm_heap *sms_heap;
/// The next heap to jump to for copying gc
extern sm_heap *sms_other_heap;
/// The singleton for true
extern sm_symbol *sms_true;
/// The singleton for false
extern sm_symbol *sms_false;

#endif /* SMS_GLOBALS */

/// Increase the global garbage collection count by 1, report the result
int sm_gc_count(int increase);
/// If replacement is non-zero, stores the new value. Returns the previous value regardless
double sm_global_growth_factor(double replacement);
/// Global space array for memory management
sm_space_array *sm_global_space_array(sm_space_array *replacement);
/// Global (primitive) function names
char *sm_global_fn_name(unsigned short int which);
/// Global function name lengths
unsigned int sm_global_fn_name_len(unsigned short int which);
/// Global number of primitive functions
unsigned int sm_global_num_fns();
/// Global lexical stack used during parsing. Before and after parsing, the top is _scratch
sm_stack *sm_global_lex_stack(sm_stack *replacement);
/// Global parser output, from the bison-generated parser
sm_object *sm_global_parser_output(sm_object *replacement);
/// Global env struct from initialization
sm_env *sm_global_environment(sm_env *replacement);
/// Global names of types
char *sm_global_type_name(unsigned short int which);
/// Global lengths of the type names
unsigned short int sm_global_type_name_len(unsigned short int which);
/// Global terminal io struct for vt100 functions
struct termios *sm_global_term_attrs();
