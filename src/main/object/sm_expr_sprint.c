// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"

// Print to string buffer the comma/semicolon separated list
uint32_t sm_expr_contents_sprint(sm_expr *expr, char *buffer, enum SM_EXPR_TYPE op, bool fake) {
  if (expr->size == 0)
    return 0;
  uint32_t buffer_pos = 0;
  uint32_t i          = op == SM_BLOCK_EXPR ? 1 : 0;
  for (; i + 1 < expr->size; i++) {
    buffer_pos += sm_object_sprint(sm_expr_get_arg(expr, i), &(buffer[buffer_pos]), fake);
    if (!fake)
      buffer[buffer_pos] = op == SM_BLOCK_EXPR ? ';' : ',';
    buffer_pos++;
  }
  if (expr->size > 0)
    buffer_pos +=
      sm_object_sprint(sm_expr_get_arg(expr, expr->size - 1), &(buffer[buffer_pos]), fake);
  return buffer_pos;
}

// Print to a cstring buffer the description of array
// Return the resulting length
uint32_t sm_expr_array_sprint(sm_expr *expr, char *buffer, bool fake) {
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
uint32_t sm_prefix_sprint(sm_expr *expr, char *buffer, bool fake) {
  if (!fake)
    sm_strncpy(buffer, sm_global_fn_name(expr->op), sm_global_fn_name_len(expr->op));
  uint32_t cursor = sm_global_fn_name_len(expr->op);
  if (!fake)
    buffer[cursor] = expr->op == SM_BLOCK_EXPR ? '{' : '(';
  cursor++;
  cursor += sm_expr_contents_sprint(expr, &(buffer[cursor]), expr->op, fake);
  if (!fake)
    buffer[cursor] = expr->op == SM_BLOCK_EXPR ? '}' : ')';
  cursor++;
  return cursor;
}

// Print description of conditional expression to buffer
// Works with if, while and doWhile expressions
uint32_t sm_ifwhile_sprint(sm_expr *expr, char *buffer, bool fake) {
  if (!fake)
    sm_strncpy(buffer, sm_global_fn_name(expr->op), sm_global_fn_name_len(expr->op));
  uint32_t cursor = sm_global_fn_name_len(expr->op);

  if (!fake)
    buffer[cursor] = '(';
  cursor++;

  cursor += sm_object_sprint(sm_expr_get_arg(expr, 0), &(buffer[cursor]), fake);

  if (!fake)
    buffer[cursor] = ')';
  cursor++;

  cursor += sm_object_sprint(sm_expr_get_arg(expr, 1), &(buffer[cursor]), fake);

  return cursor;
}

// Print description of ifelse expression to buffer
uint32_t sm_ifelse_sprint(sm_expr *expr, char *buffer, bool fake) {
  if (!fake)
    sm_strncpy(buffer, sm_global_fn_name(expr->op), sm_global_fn_name_len(expr->op));
  uint32_t cursor = sm_global_fn_name_len(expr->op);

  if (!fake)
    buffer[cursor] = '(';
  cursor++;

  cursor += sm_object_sprint(sm_expr_get_arg(expr, 0), &(buffer[cursor]), fake);

  if (!fake)
    buffer[cursor] = ')';
  cursor++;

  cursor += sm_object_sprint(sm_expr_get_arg(expr, 1), &(buffer[cursor]), fake);

  static char *else_str = " else ";
  for (int i = 0; i < 6; i++) {
    if (!fake)
      buffer[cursor] = else_str[i];
    cursor++;
  }

  cursor += sm_object_sprint(sm_expr_get_arg(expr, 2), &(buffer[cursor]), fake);

  return cursor;
}

// Print description of for expression to buffer
uint32_t sm_for_sprint(sm_expr *expr, char *buffer, bool fake) {
  if (!fake)
    sm_strncpy(buffer, sm_global_fn_name(expr->op), sm_global_fn_name_len(expr->op));
  uint32_t cursor = sm_global_fn_name_len(expr->op);

  if (!fake)
    buffer[cursor] = '(';
  cursor++;

  cursor += sm_object_sprint(sm_expr_get_arg(expr, 0), &(buffer[cursor]), fake);

  if (!fake)
    buffer[cursor] = ';';
  cursor++;


  cursor += sm_object_sprint(sm_expr_get_arg(expr, 1), &(buffer[cursor]), fake);

  if (!fake)
    buffer[cursor] = ';';
  cursor++;

  cursor += sm_object_sprint(sm_expr_get_arg(expr, 2), &(buffer[cursor]), fake);

  if (!fake)
    buffer[cursor] = ')';
  cursor++;

  cursor += sm_object_sprint(sm_expr_get_arg(expr, 3), &(buffer[cursor]), fake);

  return cursor;
}


// Print description of let expression to buffer
uint32_t sm_let_sprint(sm_expr *expr, char *buffer, bool fake) {
  if (!fake)
    sm_strncpy(buffer, sm_global_fn_name(expr->op), sm_global_fn_name_len(expr->op));
  uint32_t cursor = sm_global_fn_name_len(expr->op);

  if (!fake)
    buffer[cursor] = ' ';
  cursor++;

  cursor += sm_object_sprint(sm_expr_get_arg(expr, 0), &(buffer[cursor]), fake);

  if (!fake)
    buffer[cursor] = '=';
  cursor++;

  cursor += sm_object_sprint(sm_expr_get_arg(expr, 1), &(buffer[cursor]), fake);

  return cursor;
}


// Print infix to c string buffer
// Return length
uint32_t sm_infix_sprint(sm_expr *expr, char *buffer, bool fake) {
  if (expr->size == 0) {
    if (!fake) {
      uint32_t len = sm_global_fn_name_len(expr->op);
      buffer[0]    = '(';
      sm_strncpy(buffer + 1, sm_global_fn_name(expr->op), len);
      buffer[1 + len] = ')';
    }
    return sm_global_fn_name_len(expr->op) + 2;
  }
  if (expr->size == 1) {
    uint32_t len  = 0;
    uint32_t len2 = 0;
    len           = sm_global_fn_name_len(expr->op);
    if (!fake)
      buffer[0] = '(';
    if (!fake)
      buffer[1] = '_';
    if (!fake)
      sm_strncpy(&(buffer[2]), sm_global_fn_name(expr->op), len);

    len2 = sm_object_sprint(sm_expr_get_arg(expr, 0), buffer + 2 + len, fake);

    if (!fake)
      buffer[2 + len + len2] = ')';
    return 3 + len + len2;
  }
  if (expr->size > 2) {
    return sm_prefix_sprint(expr, buffer, fake);
  }

  sm_object *o1 = sm_expr_get_arg(expr, 0);
  sm_object *o2 = sm_expr_get_arg(expr, 1);

  // left op right
  int cursor = sm_object_sprint(o1, buffer, fake);
  if (!fake)
    sm_strncpy(&(buffer[cursor]), sm_global_fn_name(expr->op), sm_global_fn_name_len(expr->op));
  cursor += sm_global_fn_name_len(expr->op);
  cursor += sm_object_sprint(o2, &(buffer[cursor]), fake);
  return cursor;
}

// print a description of the function call to the string buffer
uint32_t sm_fun_call_sprint(sm_expr *fun_call, char *buffer, bool fake) {
  uint32_t cursor = 0;
  cursor += sm_object_sprint(sm_expr_get_arg(fun_call, 0), buffer, fake);
  cursor += sm_object_sprint(sm_expr_get_arg(fun_call, 1), &(buffer[cursor]), fake);
  return cursor;
}

// print a description of the index expression to the string buffer
uint32_t sm_index_expr_sprint(sm_expr *index_expr, char *buffer, bool fake) {
  uint32_t cursor = 0;
  cursor += sm_object_sprint(sm_expr_get_arg(index_expr, 0), buffer, fake);
  if (!fake)
    buffer[cursor] = '[';
  cursor++;
  cursor += sm_object_sprint(sm_expr_get_arg(index_expr, 1), &(buffer[cursor]), fake);
  if (!fake)
    buffer[cursor] = ']';
  cursor++;
  return cursor;
}

// Adds a c string describing the expr to the buffer
// Returns the length
uint32_t sm_expr_sprint(sm_expr *expr, char *buffer, bool fake) {
  switch (expr->op) {
  case SM_INDEX_EXPR: {
    return sm_index_expr_sprint(expr, buffer, fake);
    break;
  }
  case SM_ARRAY_EXPR: {
    return sm_expr_array_sprint(expr, buffer, fake);
    break;
  }
  case SM_FUN_CALL_EXPR: {
    return sm_fun_call_sprint(expr, buffer, fake);
    break;
  }
  case SM_PARAM_LIST_EXPR: {
    return sm_prefix_sprint(expr, buffer, fake);
    break;
  }
  case SM_IF_EXPR:
  case SM_WHILE_EXPR:
  case SM_DO_WHILE_EXPR:
    return sm_ifwhile_sprint(expr, buffer, fake);
    break;
  case SM_IF_ELSE_EXPR:
    return sm_ifelse_sprint(expr, buffer, fake);
    break;
  case SM_FOR_EXPR:
    return sm_for_sprint(expr, buffer, fake);
    break;
  case SM_LET_EXPR:
    return sm_let_sprint(expr, buffer, fake);
    break;
  default:
    if (expr->op < sm_global_num_fns()) {
      if (sm_is_infix(expr->op))
        return sm_infix_sprint(expr, buffer, fake);
      else
        return sm_prefix_sprint(expr, buffer, fake);
    } else {
      if (!fake) {
        sm_strncpy(buffer, "unrecognized expr", 17);
      }
      return 17;
    }
  }
}
