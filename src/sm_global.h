// This project is licensed under the BSD 2 clause license. See LICENSE.txt for more information.
sm_context     *sm_global_context(sm_context *replacement);
sm_memory_heap *sm_global_current_heap(sm_memory_heap *replacement);
sm_memory_heap *sm_global_other_heap(sm_memory_heap *replacement);
int             sm_gc_count(int increase);
double          sm_global_growth_factor(double replacement);
