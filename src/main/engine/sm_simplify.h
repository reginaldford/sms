// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

/// Try to simplify the expression, else return the same expression.
sm_object *sm_simplify(sm_object *seslf);
/// Whether an expression contains a specific number
bool sm_expr_has_num(sm_expr *self, f64 n);
/// Remove all cases of a certain number from this expression
sm_expr *sm_expr_rm_num(sm_expr *self, f64 v);
