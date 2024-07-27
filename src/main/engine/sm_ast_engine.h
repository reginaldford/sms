// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

/// Evaluate the input object, and return an output object
/// @param input The input object of any sms type
/// @param current_cx is the context which we are evaluating within
/// @param sf is the stack frame, which is the evaluated inputs to a function.
/// @note When an SM_SYMBOL is evaluated, it will loook into the current_cx for the variable.
/// @note If an SM_LOCAL is evaluated, it will refer to an element in the stack frame (sf).
sm_object *sm_engine_push_eval(sm_object *input, sm_cx *current_cx, sm_expr *sf);
sm_object *sm_engine_eval();
// Execute an sms function
sm_object *execute_fun(sm_fun *fun, sm_cx *current_cx, sm_expr *sf);
