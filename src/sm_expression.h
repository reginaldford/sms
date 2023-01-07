enum math_op {
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
  sm_array
};

enum print_mode {
  print_mode_immediate,
  print_mode_prefix,
  print_mode_infix,
  print_mode_postfix,
  print_mode_unknown
};

// Assumed to have size of pointers following the structure
typedef struct sm_expression {
  enum sm_object_type my_type;
  enum math_op        op;
  unsigned int        capacity;
  unsigned int        size;
} sm_expression;

sm_expression *sm_new_expression_n(enum math_op op1, unsigned int size, unsigned int capacity);
sm_expression *sm_new_expression(enum math_op op1, sm_object *arg);
sm_expression *sm_append_to_expression(sm_expression *expr, sm_object *arg);
sm_expression *sm_new_expression_2(enum math_op op1, sm_object *arg1, sm_object *arg2);
sm_expression *sm_new_expression_3(enum math_op op1, sm_object *arg1, sm_object *arg2,
                                   sm_object *arg3);
sm_string     *sm_prefix_to_string(sm_expression *expr, sm_string *op);
sm_string     *sm_infix_to_string(sm_expression *expr, sm_string *op);
sm_string     *sm_expression_to_string(sm_expression *expr);
sm_expression *sm_set_expression_arg(sm_expression *expr, unsigned int index, sm_object *value);
sm_object     *sm_get_expression_arg(sm_expression *expr, unsigned int index);
bool           sm_is_infix(enum math_op op);
