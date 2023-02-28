// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

// Denotes the type of expression
enum sm_expr_type {
  sm_plus_expr,
  sm_minus_expr,
  sm_times_expr,
  sm_divide_expr,
  sm_sqrt_expr,
  sm_sin_expr,
  sm_cos_expr,
  sm_tan_expr,
  sm_sinh_expr,
  sm_cosh_expr,
  sm_tanh_expr,
  sm_pow_expr,
  sm_csc_expr,
  sm_sec_expr,
  sm_cot_expr,
  sm_ln_expr,
  sm_exp_expr,
  sm_abs_expr,
  sm_diff_expr,
  sm_assign_expr,
  sm_test_eq_expr,
  sm_test_lt_expr,
  sm_test_gt_expr,
  sm_if_expr,
  sm_if_else_expr,
  sm_let_expr,
  sm_then_expr,
  sm_array_expr,
  sm_siblings_expr,
  sm_prim_expr,
  sm_param_list_expr,
  sm_fun_call_expr
};

// Head of an expression
// Assumed to have size of pointers following the structure
typedef struct sm_expr {
  short int         my_type;
  enum sm_expr_type op;
  unsigned int      capacity;
  unsigned int      size;
} sm_expr;

sm_expr *sm_new_expr_n(enum sm_expr_type op1, unsigned int size, unsigned int capacity);
sm_expr *sm_new_expr(enum sm_expr_type op1, sm_object *arg);
sm_expr *sm_append_to_expr(sm_expr *expr, sm_object *arg);
sm_expr *sm_new_expr_2(enum sm_expr_type op1, sm_object *arg1, sm_object *arg2);
sm_expr *sm_new_expr_3(enum sm_expr_type op1, sm_object *arg1, sm_object *arg2, sm_object *arg3);
unsigned int sm_prefix_sprint(sm_expr *self, char *buffer, bool fake);
unsigned int sm_infix_sprint(sm_expr *expr, char *buffer, bool fake);
sm_string   *sm_expr_to_string(sm_expr *expr);
unsigned int sm_expr_sprint(sm_expr *self, char *buffer, bool fake);
sm_expr     *sm_expr_set_arg(sm_expr *expr, unsigned int index, sm_object *value);
sm_object   *sm_expr_get_arg(sm_expr *expr, unsigned int index);
bool         sm_is_infix(enum sm_expr_type op);
sm_object   *sm_expr_pop(sm_expr *sme);
sm_object   *sm_expr_pop_recycle(sm_expr *sme);
