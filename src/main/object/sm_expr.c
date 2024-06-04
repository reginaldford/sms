// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"

// New expression object with n arguments
// Make sure to fill in the arguments afterward
sm_expr *sm_new_expr_n(enum SM_EXPR_TYPE op, uint32_t size, uint32_t capacity, void *notes) {
  sm_expr *new_expr  = (sm_expr *)sm_malloc(sizeof(sm_expr) + sizeof(void *) * capacity);
  new_expr->my_type  = SM_EXPR_TYPE;
  new_expr->op       = op;
  new_expr->size     = size;
  new_expr->capacity = capacity;
  new_expr->notes    = notes;
  return new_expr;
}

// New expression object without arguments
sm_expr *sm_new_expr_0(enum SM_EXPR_TYPE op, void *note) { return sm_new_expr_n(op, 0, 0, note); }

// New expression object with 1 argument
sm_expr *sm_new_expr(enum SM_EXPR_TYPE op, sm_object *arg, void *notes) {
  sm_expr *new_expr = sm_new_expr_n(op, 1, 1, notes);
  new_expr->notes   = notes;
  return sm_expr_set_arg(new_expr, 0, arg);
}

// New expression with 2 arguments
sm_expr *sm_new_expr_2(enum SM_EXPR_TYPE op, sm_object *arg1, sm_object *arg2, void *notes) {
  sm_expr *new_expr = sm_new_expr_n(op, 2, 2, notes);
  sm_expr_set_arg(new_expr, 0, arg1);
  new_expr->notes = notes;
  return sm_expr_set_arg(new_expr, 1, arg2);
}

// New expression with 3 arguments
sm_expr *sm_new_expr_3(enum SM_EXPR_TYPE op, sm_object *arg1, sm_object *arg2, sm_object *arg3,
                       void *notes) {
  sm_expr *new_expr = sm_new_expr_n(op, 3, 3, notes);
  sm_expr_set_arg(new_expr, 0, arg1);
  sm_expr_set_arg(new_expr, 1, arg2);
  new_expr->notes = notes;
  return sm_expr_set_arg(new_expr, 2, arg3);
}

// New expression with 4 arguments
sm_expr *sm_new_expr_4(enum SM_EXPR_TYPE op, sm_object *arg1, sm_object *arg2, sm_object *arg3,
                       sm_object *arg4, void *notes) {
  sm_expr *new_expr = sm_new_expr_n(op, 4, 4, notes);
  sm_expr_set_arg(new_expr, 0, arg1);
  sm_expr_set_arg(new_expr, 1, arg2);
  sm_expr_set_arg(new_expr, 2, arg3);
  new_expr->notes = notes;
  return sm_expr_set_arg(new_expr, 3, arg4);
}

// Append another object to this expression.
sm_expr *sm_expr_append(sm_expr *expr, sm_object *arg) {
  if (expr->size == expr->capacity) {
    uint32_t new_capacity = ((int)(expr->capacity * sm_global_growth_factor(0))) + 1;
    sm_expr *new_expr     = sm_new_expr_n(expr->op, expr->size + 1, new_capacity, expr->notes);
    for (uint32_t i = 0; i < expr->size; i++) {
      sm_expr_set_arg(new_expr, i, sm_expr_get_arg(expr, i));
    }
    return sm_expr_set_arg(new_expr, new_expr->size - 1, arg);
  } else {
    expr->size += 1;
    return sm_expr_set_arg(expr, expr->size - 1, arg);
  }
}

// Shallow copy
sm_expr *sm_expr_copy(sm_expr *self) {
  sm_expr *result = sm_new_expr_n(self->op, 0, self->size, self->notes);
  for (uint32_t i = 0; i < self->size; i++) {
    sm_expr_append(result, sm_expr_get_arg(self, i));
  }
  return result;
}

// Concatenate collections, assume the type of the left expression
sm_expr *sm_expr_concat(sm_expr *expr1, sm_expr *expr2) {
  sm_expr *result = sm_new_expr_n(expr1->op, 0, expr1->size + expr2->size, NULL);
  for (uint32_t i = 0; i < expr2->size; i++) {
    sm_expr_append(result, sm_expr_get_arg(expr2, i));
  }
  return result;
}

// Set an argument of an expression
sm_expr *sm_expr_set_arg(sm_expr *expr, uint32_t index, sm_object *value) {
  sm_object **ptr_array = (sm_object **)&(expr[1]);
  ptr_array[index]      = value;
  return expr;
}

// Get an argument of an expression
sm_object *sm_expr_get_arg(sm_expr *expr, uint32_t index) {
  sm_object **ptr_array = (sm_object **)&(expr[1]);
  return ptr_array[index];
}

// Can this op take 2 arguments AND have infix representation?
bool sm_is_infix(enum SM_EXPR_TYPE op) {
  switch (op) {
  case SM_ASSIGN_EXPR:
  case SM_PLUS_EXPR:
  case SM_MINUS_EXPR:
  case SM_TIMES_EXPR:
  case SM_DIVIDE_EXPR:
  case SM_POW_EXPR:
  case SM_EQEQ_EXPR:
  case SM_LT_EXPR:
  case SM_GT_EXPR:
  case SM_DOT_EXPR:
  case SM_STR_CAT_EXPR:
    return true;
  default:
    return false;
  }
}

// Useful for making decisions about parenthesis
uint16_t op_level(enum SM_EXPR_TYPE op_expr) {
  switch (op_expr) {
  case SM_PLUS_EXPR:
  case SM_MINUS_EXPR:
    return 1;
  case SM_TIMES_EXPR:
  case SM_DIVIDE_EXPR:
    return 2;
  case SM_POW_EXPR:
  case SM_EXP_EXPR:
  case SM_LN_EXPR:
    return 3;
  default:
    return 4;
  }
}

// New string describing this expression
sm_string *sm_expr_to_string(sm_expr *expr) {
  sm_string *new_str = sm_new_string_manual(sm_expr_sprint(expr, NULL, true));
  sm_expr_sprint(expr, &(new_str->content), false);
  (&new_str->content)[new_str->size] = '\0';
  return new_str;
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
