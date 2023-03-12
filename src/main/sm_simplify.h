// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

sm_object *sm_expr_simplify(sm_object *seslf);
bool       sm_expr_has_num(sm_expr *self, double n);
sm_expr   *sm_expr_rm_num(sm_expr *self, double v);
sm_object *sm_expr_simplify_once(sm_object *self);
sm_expr   *sm_expr_rm_empties(sm_expr *e);
sm_expr   *sm_expr_rm_singles(sm_expr *e);
sm_expr   *sm_expr_apply_constants(sm_expr *e);
sm_expr   *sm_expr_apply_constants2(sm_expr *e);
sm_expr   *sm_expr_apply_constants3(sm_expr *e);
sm_expr   *sm_expr_apply_constants4(sm_expr *e);
sm_expr   *sm_expr_apply_constants5(sm_expr *e);
sm_expr   *sm_expr_apply_constants6(sm_expr *e);
sm_expr   *sm_expr_apply_constants7(sm_expr *e);
