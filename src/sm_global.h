// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

sm_context     *sm_global_context(sm_context *replacement);
sm_heap        *sm_global_current_heap(sm_heap *replacement);
sm_heap        *sm_global_other_heap(sm_heap *replacement);
int             sm_gc_count(int increase);
double          sm_global_growth_factor(double replacement);
sm_space_array *sm_global_space_array(sm_space_array *replacement);
sm_expr        *sm_global_obj_stack(sm_expr *replacement);
char           *sm_global_fn_name(unsigned short int which);
unsigned int    sm_global_fn_name_len(unsigned short int which);
unsigned int    sm_global_num_fns();
sm_stack       *sm_global_lex_stack(sm_stack *replacement);
sm_expr        *sm_global_parents(sm_expr *replacement);
