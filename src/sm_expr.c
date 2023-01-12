// This project is licensed under the BSD 2 clause license. See LICENSE.txt for more information.

#include "sms.h"

// New expression object with n arguments
// Make sure to fill in the arguments afterward
sm_expr *sm_new_expr_n(enum sm_expr_type op, unsigned int size, unsigned int capacity) {
  sm_expr *new_expr  = (sm_expr *)sm_malloc(sizeof(sm_expr) + sizeof(void *) * capacity);
  new_expr->my_type  = sm_expr_type;
  new_expr->op       = op;
  new_expr->size     = size;
  new_expr->capacity = capacity;
  return new_expr;
}

// New expression object
sm_expr *sm_new_expr(enum sm_expr_type op, sm_object *arg) {
  sm_expr *new_expr = sm_new_expr_n(op, 1, 1);
  return sm_set_expr_arg(new_expr, 0, arg);
}

// Append another object to this expression.
sm_expr *sm_append_to_expr(sm_expr *expr, sm_object *arg) {
  if (expr->size == expr->capacity) {
    unsigned int new_capacity = ((int)(expr->capacity * sm_global_growth_factor(0))) + 1;
    sm_expr     *new_expr     = sm_new_expr_n(expr->op, expr->size + 1, new_capacity);
    for (unsigned int i = 0; i < expr->size; i++) {
      sm_set_expr_arg(new_expr, i, sm_get_expr_arg(expr, i));
    }
    return sm_set_expr_arg(new_expr, new_expr->size - 1, arg);
  } else {
    expr->size += 1;
    return sm_set_expr_arg(expr, expr->size - 1, arg);
  }
}

// New expression with 2 arguments
sm_expr *sm_new_expr_2(enum sm_expr_type op, sm_object *arg1, sm_object *arg2) {
  sm_expr *new_expr = sm_new_expr_n(op, 2, 2);
  sm_set_expr_arg(new_expr, 0, arg1);
  return sm_set_expr_arg(new_expr, 1, arg2);
}

// New expression with 3 arguments
sm_expr *sm_new_expr_3(enum sm_expr_type op, sm_object *arg1, sm_object *arg2, sm_object *arg3) {
  sm_expr *new_expr = sm_new_expr_n(op, 2, 2);
  sm_set_expr_arg(new_expr, 0, arg1);
  sm_set_expr_arg(new_expr, 1, arg2);
  return sm_set_expr_arg(new_expr, 2, arg3);
}

// Set an argument of an expression
sm_expr *sm_set_expr_arg(sm_expr *expr, unsigned int index, sm_object *value) {
  sm_object **ptr_array = (sm_object **)&(expr[1]);
  ptr_array[index]      = value;
  return expr;
}

// Get an argument of an expression
sm_object *sm_get_expr_arg(sm_expr *expr, unsigned int index) {
  sm_object **ptr_array = (sm_object **)&(expr[1]);
  return ptr_array[index];
}

// Can this op take 2 arguments AND have infix representation?
bool sm_is_infix(enum sm_expr_type op) {
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

// New string containing the contents of an expression without the bounding symbols
sm_string *sm_expr_contents_to_string(sm_expr *sme) {
  sm_string *result_str;
  if (sme->size > 0)
    result_str = sm_object_to_string(sm_get_expr_arg(sme, 0));
  else
    return sm_new_string(0, "");
  for (unsigned int arg_index = 1; arg_index < sme->size; arg_index++) {
    result_str         = sm_string_add_recycle(result_str, sm_new_string(1, ","));
    sm_string *obj_str = sm_object_to_string(sm_get_expr_arg(sme, arg_index));
    result_str         = sm_string_add_recycle(result_str, obj_str);
  }
  return result_str;
}


// New string containing array description
sm_string *sm_expr_array_to_string(sm_expr *expr) {
  if (expr->size == 0)
    return sm_new_string(2, "[]");
  sm_string *str_beginning = sm_new_string(1, "[");
  sm_string *str_contents  = sm_expr_contents_to_string(expr);
  sm_string *result_str    = sm_string_add_recycle(str_beginning, str_contents);
  return sm_string_add_recycle(result_str, sm_new_string(1, "]"));
}


// New string containing prefix description
sm_string *sm_prefix_to_string(sm_expr *expr, sm_string *op) {
  if (expr->size == 0)
    return sm_string_add_recycle(op, sm_new_string(2, "()"));
  sm_string *str_beginning = sm_string_add_recycle(op, sm_new_string(1, "("));
  sm_string *str_contents  = sm_expr_contents_to_string(expr);
  sm_string *result_str    = sm_string_add_recycle(str_beginning, str_contents);
  return sm_string_add_recycle(result_str, sm_new_string(1, ")"));
}


// New String containing infix description
// If there are more than 2 arguments, defaults to prefix
// Adding an arg to 1 + 1 leads to +( 1, 1, x )
sm_string *sm_infix_to_string(sm_expr *expr, sm_string *op) {
  if (expr->size > 2)
    return sm_prefix_to_string(expr, op);

  sm_object *o1 = sm_get_expr_arg(expr, 0);
  sm_object *o2 = sm_get_expr_arg(expr, 1);

  sm_string *left_string  = sm_object_to_string(o1);
  sm_string *right_string = sm_object_to_string(o2);

  sm_string *str;

  if (o1->my_type == sm_expr_type && sm_is_infix(((sm_expr *)o1)->op) &&
      o2->my_type == sm_expr_type && sm_is_infix(((sm_expr *)o2)->op)) {
    str = sm_string_add_recycle_1st(sm_new_string(1, "("), left_string);
    str = sm_string_add_recycle(str, sm_new_string(1, ")"));
    str = sm_string_add_recycle(str, op);
    str = sm_string_add_recycle(str, sm_new_string(1, "("));
    str = sm_string_add_recycle(str, right_string);
    return sm_string_add_recycle(str, sm_new_string(1, ")"));
  } else if (o1->my_type == sm_expr_type && sm_is_infix(((sm_expr *)o1)->op)) {
    str = sm_string_add_recycle(sm_new_string(1, "("), left_string);
    str = sm_string_add_recycle(str, sm_new_string(1, ")"));
    str = sm_string_add_recycle(str, op);
    return sm_string_add_recycle(str, right_string);
  } else if (o2->my_type == sm_expr_type && sm_is_infix(((sm_expr *)o2)->op)) {
    str = sm_string_add_recycle(op, sm_new_string(1, "("));
    str = sm_string_add_recycle(str, sm_new_string(1, "("));
    str = sm_string_add_recycle(str, right_string);
    return sm_string_add_recycle(str, sm_new_string(1, ")"));
  } else {
    sm_string *left_and_op = sm_string_add_recycle(left_string, op);
    return sm_string_add_recycle(left_and_op, right_string);
  }
}

// New string describing this expression
sm_string *sm_expr_to_string(sm_expr *expr) {
  static char             *response[]     = {"+",   "-",   "*",    "/",    "=",    "sqrt", "sin",
                                             "cos", "tan", "sinh", "cosh", "tanh", "pow",  "csc",
                                             "sec", "cot", "ln",   "exp",  "diff"};
  static long unsigned int response_len[] = {1, 1, 1, 1, 1, 4, 3, 3, 3, 4,
                                             4, 4, 3, 3, 3, 3, 2, 3, 4};

  if (expr->op == sm_array) {
    return sm_expr_array_to_string(expr);
  } else {
    if (expr->op < ARRAY_SIZE(response) && expr->op) {
      sm_string *op_string = sm_new_string(response_len[expr->op], response[expr->op]);
      if (sm_is_infix(expr->op))
        return sm_infix_to_string(expr, op_string);
      else
        return sm_prefix_to_string(expr, op_string);
    } else {
      return sm_new_string(17, "unrecognized expr");
    }
  }
}

// Remove and return the object with the highest index
sm_object *sm_expr_pop(sm_expr *sme) {
  if (sme->size > 0) {
    sme->size--;
    return sm_get_expr_arg(sme, sme->size);
  } else {
    printf("Stack underflow.");
    return NULL;
  }
}

// Remove and return the object with the highest index,
// then, place a space object in the remaining space
sm_object *sm_expr_pop_recycle(sm_expr *sme) {
  if (sme->size > 0) {
    sme->size--;
    sm_object *answer = sm_get_expr_arg(sme, sme->size);
    sm_new_space_after(sme, sizeof(sm_object *));
    return answer;
  } else {
    printf("Stack underflow.");
    return NULL;
  }
}
