// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

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
      sm_set_expr_arg(new_expr, i, sm_expr_get_arg(expr, i));
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
  sm_expr *new_expr = sm_new_expr_n(op, 3, 3);
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
sm_object *sm_expr_get_arg(sm_expr *expr, unsigned int index) {
  sm_object **ptr_array = (sm_object **)&(expr[1]);
  return ptr_array[index];
}

// Can this op take 2 arguments AND have infix representation?
bool sm_is_infix(enum sm_expr_type op) {
  switch (op) {
  case sm_assign:
  case sm_plus:
  case sm_minus:
  case sm_times:
  case sm_divide:
  case sm_pow:
  case sm_test_eq:
  case sm_test_lt:
  case sm_test_gt:
    return true;
  default:
    return false;
  }
}

unsigned int sm_expr_contents_sprint(sm_expr *expr, char *buffer, enum sm_expr_type op, bool fake) {
  if (expr->size == 0)
    return 0;
  unsigned int buffer_pos = 0;
  for (unsigned int i = 0; i + 1 < expr->size; i++) {
    buffer_pos += sm_object_sprint(sm_expr_get_arg(expr, i), &(buffer[buffer_pos]), fake);
    if (op != sm_then) {
      if (!fake)
        buffer[buffer_pos] = ',';
      buffer_pos++;
    } else if (!fake) {
      if (!fake)
        buffer[buffer_pos] = ';';
      buffer_pos++;
    }
  }
  buffer_pos +=
    sm_object_sprint(sm_expr_get_arg(expr, expr->size - 1), &(buffer[buffer_pos]), fake);
  return buffer_pos;
}


// Print to a cstring buffer the description of array
// Return the resulting length
unsigned int sm_expr_array_sprint(sm_expr *expr, char *buffer, bool fake) {
  if (!fake)
    buffer[0] = '[';
  if (expr->size == 0) {
    if (!fake)
      buffer[1] = ']';
    return 2;
  }
  int len = sm_expr_contents_sprint(expr, &(buffer[1]), expr->op, fake);
  if (!fake)
    buffer[1 + len] = ']';
  return 2 + len;
}

// Print description of prefix expression to buffer
unsigned int sm_prefix_sprint(sm_expr *expr, char *buffer, bool fake) {
  if (expr->size == 0) {
    if (!fake)
      buffer[0] = '[';
    if (!fake)
      buffer[1] = ']';
    return 2;
  }
  if (!fake)
    sm_strncpy(buffer, sm_global_fn_name(expr->op), sm_global_fn_name_len(expr->op));
  unsigned int cursor = sm_global_fn_name_len(expr->op);
  if (!fake)
    buffer[cursor] = '(';
  cursor++;
  cursor += sm_expr_contents_sprint(expr, &(buffer[cursor]), expr->op, fake);
  if (!fake)
    buffer[cursor] = ')';
  cursor++;
  return cursor;
}

// Useful for making decisions about parenthesis
unsigned short int op_level(enum sm_expr_type op_type) {
  switch (op_type) {
  case sm_plus:
  case sm_minus:
    return 1;
  case sm_times:
  case sm_divide:
    return 2;
  case sm_pow:
  case sm_exp:
  case sm_ln:
    return 3;
  default:
    return 4;
  }
}

// Print infix to c string buffer
// Return length
unsigned int sm_infix_sprint(sm_expr *expr, char *buffer, bool fake) {
  if (expr->size > 2) {
    return sm_prefix_sprint(expr, buffer, fake);
  }

  sm_object *o1 = sm_expr_get_arg(expr, 0);
  sm_object *o2 = sm_expr_get_arg(expr, 1);

  int mid_op_level   = op_level(expr->op);
  int left_op_level  = o1->my_type == sm_expr_type ? op_level(((sm_expr *)o1)->op) : 5;
  int right_op_level = o2->my_type == sm_expr_type ? op_level(((sm_expr *)o1)->op) : 5;

  if (left_op_level <= 2 && right_op_level <= 2 && left_op_level != mid_op_level) {
    //(left)op(right)
    if (!fake)
      buffer[0] = '(';
    int cursor = 1 + sm_object_sprint(o1, &(buffer[1]), fake);
    if (!fake)
      buffer[cursor] = ')';
    cursor++;
    if (!fake)
      sm_strncpy(&(buffer[cursor]), sm_global_fn_name(expr->op), sm_global_fn_name_len(expr->op));
    cursor += sm_global_fn_name_len(expr->op);
    if (!fake)
      buffer[cursor] = '(';
    cursor++;
    cursor += sm_object_sprint(o2, &(buffer[cursor]), fake);
    if (!fake)
      buffer[cursor] = ')';
    cursor++;
    return cursor;
  } else if (left_op_level <= 2 && left_op_level != mid_op_level) {
    //(left)op right
    if (!fake)
      buffer[0] = '(';
    int cursor = 1 + sm_object_sprint(o1, &(buffer[1]), fake);
    if (!fake)
      buffer[cursor] = ')';
    cursor++;
    if (!fake)
      sm_strncpy(&(buffer[cursor]), sm_global_fn_name(expr->op), sm_global_fn_name_len(expr->op));
    cursor += sm_object_sprint(o2, &(buffer[cursor]), fake);
    return cursor;
  } else if (right_op_level <= 2 && right_op_level != mid_op_level) {
    // left op(right)
    int cursor = sm_object_sprint(o1, buffer, fake);
    if (!fake)
      sm_strncpy(&(buffer[cursor]), sm_global_fn_name(expr->op), sm_global_fn_name_len(expr->op));
    cursor += sm_global_fn_name_len(expr->op);
    if (!fake)
      buffer[cursor] = '(';
    cursor++;
    cursor += sm_object_sprint(o2, &(buffer[cursor]), fake);
    if (!fake)
      buffer[cursor] = ')';
    cursor++;
    return cursor;
  } else {
    // left op right
    int cursor = sm_object_sprint(o1, buffer, fake);
    if (!fake)
      sm_strncpy(&(buffer[cursor]), sm_global_fn_name(expr->op), sm_global_fn_name_len(expr->op));
    cursor += sm_global_fn_name_len(expr->op);
    cursor += sm_object_sprint(o2, &(buffer[cursor]), fake);
    return cursor;
  }
}

// New string describing this expression
sm_string *sm_expr_to_string(sm_expr *expr) {
  sm_string *new_str = sm_new_string(sm_expr_sprint(expr, NULL, true), "");
  sm_expr_sprint(expr, &(new_str->content), false);
  (&new_str->content)[new_str->size] = '\0';
  return new_str;
}

unsigned int sm_fun_call_sprint(sm_expr *fun_call, char *buffer, bool fake) {
  unsigned int cursor = 0;
  cursor += sm_object_sprint(sm_expr_get_arg(fun_call, 0), buffer, fake);
  cursor += sm_object_sprint(sm_expr_get_arg(fun_call, 1), &(buffer[cursor]), fake);
  return cursor;
}

// Adds a c string describing the expr to the buffer
// Returns the length
unsigned int sm_expr_sprint(sm_expr *expr, char *buffer, bool fake) {
  if (expr->op == sm_array) {
    return sm_expr_array_sprint(expr, buffer, fake);
  } else {
    if (expr->op < sm_global_num_fns()) {
      if (sm_is_infix(expr->op))
        return sm_infix_sprint(expr, buffer, fake);
      else
        return sm_prefix_sprint(expr, buffer, fake);
    } else if (expr->op == sm_fun_call) {
      return sm_fun_call_sprint(expr, buffer, fake);
    } else {
      if (!fake)
        sm_strncpy(buffer, "unrecognized expr", 17);
      return 17;
    }
  }
}

// Remove and return the object with the highest index
sm_object *sm_expr_pop(sm_expr *sme) {
  if (sme->size > 0) {
    sme->size--;
    return sm_expr_get_arg(sme, sme->size);
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
    sme->capacity     = sme->size;
    sm_object *answer = sm_expr_get_arg(sme, sme->size);
    sm_new_space_after(sme, sizeof(sm_object *));
    return answer;
  } else {
    printf("Stack underflow.");
    return NULL;
  }
}
