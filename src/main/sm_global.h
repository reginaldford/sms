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
/// Explicit callstack
extern sm_stack *sms_callstack;
#endif /* SMS_GLOBALS */

/// Increase the global garbage collection count by 1, report the result
int sm_gc_count(int increase);
/// If replacement is non-zero, stores the new value. Returns the previous value regardless
double sm_global_growth_factor(double replacement);
/// Global (primitive) function names
char *sm_global_fn_name(uint32_t which);
/// Global function name lengths
uint32_t sm_global_fn_name_len(uint32_t which);
/// Global number of primitive functions
uint32_t sm_global_num_fns();
/// Global lexical stack used during parsing. Before and after parsing, the top is _scratch
sm_stack *sm_global_lex_stack(sm_stack *replacement);
/// Global parser output, from the bison-generated parser
sm_object *sm_global_parser_output(sm_object *replacement);
/// Global env struct from initialization
sm_env *sm_global_environment(sm_env *replacement);
/// Global terminal io struct for vt100 functions
struct termios *sm_global_term_attrs();
/// Read-only for user home directory
struct sm_string *sm_global_home_directory();
/// Global version string
sm_string *sms_global_version();
/// Returns whether fn name is hidden when containing expression is printed
bool sm_global_fn_hidden(uint32_t which);
