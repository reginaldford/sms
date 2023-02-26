// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

// Denotes the type of expression
enum sm_expr_type {
  sm_plus,
  sm_minus,
  sm_times,
  sm_divide,
  sm_sqrt,
  sm_sin,
  sm_cos,
  sm_tan,
  sm_sinh,
  sm_cosh,
  sm_tanh,
  sm_pow,
  sm_csc,
  sm_sec,
  sm_cot,
  sm_ln,
  sm_exp,
  sm_abs,
  sm_diff,
  sm_assign,
  sm_test_eq,
  sm_test_lt,
  sm_test_gt,
  sm_if,
  sm_if_else,
  sm_let,
  sm_then,
  sm_array,
  sm_siblings,
  sm_prim,
  sm_param_list,
  sm_fun_call
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
sm_expr     *sm_set_expr_arg(sm_expr *expr, unsigned int index, sm_object *value);
sm_object   *sm_expr_get_arg(sm_expr *expr, unsigned int index);
bool         sm_is_infix(enum sm_expr_type op);
sm_object   *sm_expr_pop(sm_expr *sme);
sm_object   *sm_expr_pop_recycle(sm_expr *sme);
