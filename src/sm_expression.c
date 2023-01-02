#include "sms.h"

sm_expression *sm_new_expression(enum math_op op1, sm_object *arg) {
  sm_expression *new_expr = (sm_expression *)sm_malloc(sizeof(sm_expression) + sizeof(void *));
  new_expr->my_type       = sm_expression_type;
  new_expr->op            = op1;
  new_expr->num_args      = 1;
  new_expr->capacity      = 1;
  return sm_set_expression_arg(new_expr, 0, arg);
}

// make sure to put the arguments in afterward.
sm_expression *sm_new_expression_n(enum math_op op1, int capacity, int num_args) {
  sm_expression *new_expr =
      (sm_expression *)sm_malloc(sizeof(sm_expression) + sizeof(void *) * capacity);
  new_expr->my_type  = sm_expression_type;
  new_expr->op       = op1;
  new_expr->num_args = num_args;
  new_expr->capacity = capacity;
  return new_expr;
}

sm_expression *sm_append_to_expression(sm_expression *expr, sm_object *arg) {
  if (expr->num_args == expr->capacity) {
    int new_capacity        = ((int)(expr->capacity * sm_collection_growth_factor(0))) + 1;
    sm_expression *new_expr = sm_new_expression_n(expr->op, new_capacity, expr->num_args + 1);
    new_expr = (sm_expression *)sm_malloc(sizeof(sm_expression) + sizeof(void *) * new_capacity);
    new_expr->my_type  = sm_expression_type;
    new_expr->op       = expr->op;
    new_expr->num_args = expr->num_args + 1;
    new_expr->capacity = new_capacity;
    for (int i = 0; i < expr->num_args; i++) {
      sm_set_expression_arg(new_expr, i, sm_get_expression_arg(expr, i));
    }
    return sm_set_expression_arg(new_expr, new_expr->num_args - 1, arg);
  } else {
    expr->num_args += 1;
    return sm_set_expression_arg(expr, expr->num_args - 1, arg);
  }
}

sm_expression *sm_new_expression2(enum math_op op1, sm_object *arg1, sm_object *arg2) {
  sm_expression *new_expr = (sm_expression *)sm_malloc(sizeof(sm_expression) + sizeof(void *) * 2);
  new_expr->my_type       = sm_expression_type;
  new_expr->op            = op1;
  new_expr->num_args      = 2;
  new_expr->capacity      = 2;
  sm_set_expression_arg(new_expr, 0, arg1);
  return sm_set_expression_arg(new_expr, 1, arg2);
}

sm_expression *sm_new_expression3(enum math_op op1, sm_object *arg1, sm_object *arg2,
                                  sm_object *arg3) {
  sm_expression *new_expr = (sm_expression *)sm_malloc(sizeof(sm_expression) + sizeof(void *) * 3);
  new_expr->my_type       = sm_expression_type;
  new_expr->op            = op1;
  new_expr->num_args      = 3;
  new_expr->capacity      = 3;
  sm_set_expression_arg(new_expr, 0, arg1);
  sm_set_expression_arg(new_expr, 1, arg2);
  return sm_set_expression_arg(new_expr, 2, arg3);
}

sm_expression *sm_set_expression_arg(sm_expression *expr, unsigned int index, sm_object *value) {
  sm_object **ptr_array = (sm_object **)&(expr[1]);
  ptr_array[index]      = value;
  return expr;
}

sm_object *sm_get_expression_arg(sm_expression *expr, unsigned int index) {
  sm_object **ptr_array = (sm_object **)&(expr[1]);
  return ptr_array[index];
}

bool sm_is_infix(enum math_op op) {
  switch (op) {
  case sm_plus:
    return true;
  case sm_minus:
    return true;
  case sm_times:
    return true;
  case sm_divide:
    return true;
  case sm_pow:
    return true;
  case sm_sin:
    return false;
  case sm_cos:
    return false;
  case sm_tan:
    return false;
  case sm_sec:
    return false;
  case sm_csc:
    return false;
  case sm_cot:
    return false;
  case sm_sinh:
    return false;
  case sm_cosh:
    return false;
  case sm_tanh:
    return false;
  case sm_ln:
    return false;
  case sm_exp:
    return false;
  case sm_sqrt:
    return false;
  default:
    return NULL;
  }
}

sm_string *sm_prefix_to_string(sm_expression *expr, sm_string *op) {
  int arg_length_sum = 0;
  sm_string *arg_strings[expr->num_args];
  for (int arg_index = 0; arg_index < expr->num_args; arg_index++) {
    sm_object *o1          = sm_get_expression_arg(expr, arg_index);
    arg_strings[arg_index] = sm_object_to_string(o1);
    arg_length_sum += arg_strings[arg_index]->size;
  }
  // 2 surrounding spaces and 2 separating characters
  arg_length_sum += 2 + 2 * (expr->num_args - 1);

  //(spaced operators)+(spaced arguments)+ (NULL character)
  int answer_length = op->size + 2 + arg_length_sum + expr->num_args * 2 + 1;
  char *buf         = malloc(sizeof(char) * answer_length);
  sprintf(buf, "%s( ", &(op->content));
  int buffer_pos = op->size + 2;
  for (int arg_index = 0; arg_index < expr->num_args - 1; arg_index++) {
    sprintf(buf + buffer_pos, "%s, ", &(arg_strings[arg_index]->content));
    buffer_pos += arg_strings[arg_index]->size + 2;
  }
  sprintf(buf + buffer_pos, "%s )", &(arg_strings[expr->num_args - 1]->content));
  // temporary fix. Need to use sm_strcat etc. to avoid strlen
  answer_length     = strlen(buf);
  sm_string *answer = sm_new_string(answer_length, buf);
  free(buf);
  return answer;
}

sm_string *sm_infix_to_string(sm_expression *expr, sm_string *op) {
  // If we have more than 2, it's prefix.
  // So, adding an arg to 1 + 1 => +( 1, 1, x )
  if (expr->num_args > 2)
    return sm_prefix_to_string(expr, op);

  sm_object *o1 = sm_get_expression_arg(expr, 0);
  sm_object *o2 = sm_get_expression_arg(expr, 1);

  sm_string *left_sm_string  = sm_object_to_string(o1);
  sm_string *right_sm_string = sm_object_to_string(o2);

  char *op_str      = &(op->content);
  char *left_str    = &(left_sm_string->content);
  char *right_str   = &(right_sm_string->content);
  int string_length = 0;
  char *buf;
  if (o1->my_type == sm_expression_type && sm_is_infix(((sm_expression *)o1)->op) &&
      o2->my_type == sm_expression_type && sm_is_infix(((sm_expression *)o2)->op)) {
    string_length = left_sm_string->size + right_sm_string->size + 13;
    buf           = malloc(sizeof(char) * string_length + 1);
    sprintf(buf, "( %s ) %s ( %s )", left_str, op_str, right_str);
  } else if (o1->my_type == sm_expression_type && sm_is_infix(((sm_expression *)o1)->op)) {
    string_length = left_sm_string->size + right_sm_string->size + 9;
    buf           = malloc(sizeof(char) * string_length + 1);
    sprintf(buf, "( %s ) %s %s", left_str, op_str, right_str);
  } else if (o2->my_type == sm_expression_type && sm_is_infix(((sm_expression *)o2)->op)) {
    string_length = left_sm_string->size + right_sm_string->size + 9;
    buf           = malloc(sizeof(char) * string_length + 1);
    sprintf(buf, "%s %s ( %s )", left_str, op_str, right_str);
  } else {
    string_length = left_sm_string->size + right_sm_string->size + 5;
    buf           = malloc(sizeof(char) * string_length + 1);
    sprintf(buf, "%s %s %s", left_str, op_str, right_str);
  }
  sm_string *answer = sm_new_string(string_length, buf);
  free(buf);
  return answer;
}

sm_string *sm_expression_to_string(sm_expression *expr) {
  switch (expr->op) {
  case sm_plus:
    return sm_infix_to_string(expr, sm_new_string(1, "+"));
  case sm_minus:
    return sm_infix_to_string(expr, sm_new_string(1, "-"));
  case sm_times:
    return sm_infix_to_string(expr, sm_new_string(1, "*"));
  case sm_divide:
    return sm_infix_to_string(expr, sm_new_string(1, "/"));
  case sm_pow:
    return sm_infix_to_string(expr, sm_new_string(1, "^"));
  case sm_sin:
    return sm_prefix_to_string(expr, sm_new_string(3, "sin"));
  case sm_cos:
    return sm_prefix_to_string(expr, sm_new_string(3, "cos"));
  case sm_tan:
    return sm_prefix_to_string(expr, sm_new_string(3, "tan"));
  case sm_sec:
    return sm_prefix_to_string(expr, sm_new_string(3, "sec"));
  case sm_csc:
    return sm_prefix_to_string(expr, sm_new_string(3, "csc"));
  case sm_cot:
    return sm_prefix_to_string(expr, sm_new_string(3, "cot"));
  case sm_sinh:
    return sm_prefix_to_string(expr, sm_new_string(4, "sinh"));
  case sm_cosh:
    return sm_prefix_to_string(expr, sm_new_string(4, "cosh"));
  case sm_tanh:
    return sm_prefix_to_string(expr, sm_new_string(4, "tanh"));
  case sm_ln:
    return sm_prefix_to_string(expr, sm_new_string(2, "ln"));
  case sm_exp:
    return sm_prefix_to_string(expr, sm_new_string(3, "exp"));
  case sm_sqrt:
    return sm_prefix_to_string(expr, sm_new_string(4, "sqrt"));
  case sm_diff:
    return sm_prefix_to_string(expr, sm_new_string(4, "diff"));
  default:
    return sm_new_string(2, "??");
  }
}
