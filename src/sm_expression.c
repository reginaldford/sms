// This project is licensed under the BSD 2 clause license. See LICENSE.txt for more information.
#include "sms.h"

// make sure to put the arguments in afterward.
sm_expression *sm_new_expression_n(enum math_op op, unsigned int size, unsigned int capacity) {
  sm_expression *new_expr =
    (sm_expression *)sm_malloc(sizeof(sm_expression) + sizeof(void *) * capacity);
  new_expr->my_type  = sm_expression_type;
  new_expr->op       = op;
  new_expr->size     = size;
  new_expr->capacity = capacity;
  return new_expr;
}

sm_expression *sm_new_expression(enum math_op op, sm_object *arg) {
  sm_expression *new_expr = sm_new_expression_n(op, 1, 1);
  return sm_set_expression_arg(new_expr, 0, arg);
}

sm_expression *sm_append_to_expression(sm_expression *expr, sm_object *arg) {
  if (expr->size == expr->capacity) {
    unsigned int   new_capacity = ((int)(expr->capacity * sm_global_growth_factor(0))) + 1;
    sm_expression *new_expr     = sm_new_expression_n(expr->op, expr->size + 1, new_capacity);
    for (unsigned int i = 0; i < expr->size; i++) {
      sm_set_expression_arg(new_expr, i, sm_get_expression_arg(expr, i));
    }
    return sm_set_expression_arg(new_expr, new_expr->size - 1, arg);
  } else {
    expr->size += 1;
    return sm_set_expression_arg(expr, expr->size - 1, arg);
  }
}

sm_expression *sm_new_expression_2(enum math_op op, sm_object *arg1, sm_object *arg2) {
  sm_expression *new_expr = sm_new_expression_n(op, 2, 2);
  sm_set_expression_arg(new_expr, 0, arg1);
  return sm_set_expression_arg(new_expr, 1, arg2);
}

sm_expression *sm_new_expression_3(enum math_op op, sm_object *arg1, sm_object *arg2,
                                   sm_object *arg3) {
  sm_expression *new_expr = sm_new_expression_n(op, 2, 2);
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
  case sm_array:
    return false;
  default:
    return NULL;
  }
}

sm_string *sm_prefix_to_string(sm_expression *expr, sm_string *op) {
  sm_string *str;

  if (expr->op == sm_array) {
    str = sm_new_string(2, "[ ");
  } else {
    str = sm_concat_strings_recycle_2nd(op, sm_new_string(2, "( "));
  }

  for (unsigned int arg_index = 0; arg_index + 1 < expr->size; arg_index++) {
    sm_string *obj_str = sm_object_to_string(sm_get_expression_arg(expr, arg_index));
    str                = sm_concat_strings(str, obj_str);
    str                = sm_concat_strings_recycle(str, sm_new_string(2, ", "));
  }

  if (expr->size > 0) {
    sm_string *obj_str = sm_object_to_string(sm_get_expression_arg(expr, expr->size - 1));
    str                = sm_concat_strings_recycle_1st(str, obj_str);
    if (expr->op == sm_array) {
      str = sm_concat_strings_recycle(str, sm_new_string(2, " ]"));
    } else
      str = sm_concat_strings_recycle(str, sm_new_string(2, " )"));
  } else { // size is 0
    if (expr->op == sm_array) {
      str = sm_concat_strings_recycle(str, sm_new_string(1, "]"));
    } else
      str = sm_concat_strings_recycle(str, sm_new_string(1, ")"));
  }
  return str;
}

sm_string *sm_infix_to_string(sm_expression *expr, sm_string *op) {
  // If we have more than 2, it's prefix.
  // So, adding an arg to 1 + 1 => +( 1, 1, x )
  if (expr->size > 2)
    return sm_prefix_to_string(expr, op);

  sm_object *o1 = sm_get_expression_arg(expr, 0);
  sm_object *o2 = sm_get_expression_arg(expr, 1);

  sm_string *left_string  = sm_object_to_string(o1);
  sm_string *right_string = sm_object_to_string(o2);

  sm_string *str;

  if (o1->my_type == sm_expression_type && sm_is_infix(((sm_expression *)o1)->op) &&
      o2->my_type == sm_expression_type && sm_is_infix(((sm_expression *)o2)->op)) {
    str = sm_concat_strings_recycle_1st(sm_new_string(2, "( "), left_string);
    str = sm_concat_strings_recycle(str, sm_new_string(3, " ) "));
    str = sm_concat_strings_recycle_1st(str, op);
    str = sm_concat_strings_recycle(str, sm_new_string(2, " ( "));
    str = sm_concat_strings_recycle_1st(str, right_string);
    str = sm_concat_strings_recycle(str, sm_new_string(2, " )"));
  } else if (o1->my_type == sm_expression_type && sm_is_infix(((sm_expression *)o1)->op)) {
    str = sm_concat_strings_recycle_1st(sm_new_string(2, "( "), left_string);
    str = sm_concat_strings_recycle(str, sm_new_string(3, " ) "));
    str = sm_concat_strings_recycle_1st(str, op);
    str = sm_concat_strings_recycle(str, sm_new_string(1, " "));
    str = sm_concat_strings_recycle_1st(str, right_string);
  } else if (o2->my_type == sm_expression_type && sm_is_infix(((sm_expression *)o2)->op)) {
    str = sm_concat_strings_recycle_2nd(left_string, sm_new_string(1, " "));
    str = sm_concat_strings_recycle_1st(str, op);
    str = sm_concat_strings_recycle(str, sm_new_string(2, " ( "));
    str = sm_concat_strings_recycle_1st(str, right_string);
    str = sm_concat_strings_recycle(str, sm_new_string(2, " )"));
  } else {
    str = sm_concat_strings_recycle(left_string, sm_new_string(1, " "));
    str = sm_concat_strings_recycle_1st(str, op);
    str = sm_concat_strings_recycle(str, sm_new_string(1, " "));
    str = sm_concat_strings_recycle(str, right_string);
  }
  return str;
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
  case sm_array:
    return sm_prefix_to_string(expr, sm_new_string(5, "array"));
  default:
    return sm_new_string(2, "??");
  }
}
