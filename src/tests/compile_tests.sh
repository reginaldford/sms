clang -fshort-enums sanity_tests.c -o sanity_tests
clang memory_show.c ../sm_heap.c ../sm_global.c ../sm_gc.c  ../sm_object.c ../sm_string.c ../sm_expr.c ../sm_context.c ../sm_double.c ../sm_symbol.c ../sm_meta.c ../sm_pointer.c ../sm_space.c ../sm_signal.c ../sm_stack.c -o memory_show
