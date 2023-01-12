// This project is licensed under the BSD 2 clause license. See LICENSE.txt for more information.

sm_context     *sm_global_context(sm_context *replacement);
sm_heap        *sm_global_current_heap(sm_heap *replacement);
sm_heap        *sm_global_other_heap(sm_heap *replacement);
int             sm_gc_count(int increase);
double          sm_global_growth_factor(double replacement);
sm_space_array *sm_global_space_array(sm_space_array *replacement);
sm_expr        *sm_global_obj_stack(sm_expr *replacement);
