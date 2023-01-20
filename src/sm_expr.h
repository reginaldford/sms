// The following file is provided under the BSD 2-clause license. For more info, read LICENSE.txt.

// Denotes the type of expression
enum sm_expr_type {
  sm_plus,
  sm_minus,
  sm_times,
  sm_divide,
  sm_equals,
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
  sm_diff,
  sm_array,
  sm_if,
  sm_if_else,
  sm_funcall_v_l,
  sm_funcall_v_v,
  sm_funcall_l_v,
  sm_funcall_l_l,
  sm_test_eq,
  sm_test_lt,
  sm_test_mt,
  sm_let,
  sm_assign,
  sm_cprim
};

// Head of an expression
// Assumed to have size of pointers following the structure
typedef struct sm_expr {
  short int         my_type;
  enum sm_expr_type op;
  unsigned int      capacity;
  unsigned int      size;
} sm_expr;

sm_expr   *sm_new_expr_n(enum sm_expr_type op1, unsigned int size, unsigned int capacity);
sm_expr   *sm_new_expr(enum sm_expr_type op1, sm_object *arg);
sm_expr   *sm_append_to_expr(sm_expr *expr, sm_object *arg);
sm_expr   *sm_new_expr_2(enum sm_expr_type op1, sm_object *arg1, sm_object *arg2);
sm_expr   *sm_new_expr_3(enum sm_expr_type op1, sm_object *arg1, sm_object *arg2, sm_object *arg3);
sm_string *sm_prefix_to_string(sm_expr *expr, sm_string *op);
unsigned int sm_prefix_sprint(sm_expr *self, char *buffer);
sm_string   *sm_infix_to_string(sm_expr *expr, sm_string *op);
unsigned int sm_infix_sprint(sm_expr *expr, char *buffer);
sm_string   *sm_expr_to_string(sm_expr *expr);
unsigned int sm_expr_sprint(sm_expr *self, char *buffer);
unsigned int sm_expr_to_string_len(sm_expr *expr);
sm_expr     *sm_set_expr_arg(sm_expr *expr, unsigned int index, sm_object *value);
sm_object   *sm_get_expr_arg(sm_expr *expr, unsigned int index);
bool         sm_is_infix(enum sm_expr_type op);
sm_object   *sm_expr_pop(sm_expr *sme);
sm_object   *sm_expr_pop_recycle(sm_expr *sme);
