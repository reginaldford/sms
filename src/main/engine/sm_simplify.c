// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"

// Whether an expression contains a specific f64
bool sm_expr_has_num(sm_expr *expr, double n) {
  for (uint32_t i = 0; i < expr->size; i++) {
    sm_object *current_obj = sm_expr_get_arg(expr, i);
    if (current_obj->my_type == SM_F64_TYPE) {
      if (((sm_f64 *)current_obj)->value == n)
        return true;
    }
  }
  return false;
}

// Remove all cases of a certain f64 from this expression
sm_expr *sm_expr_rm_num(sm_expr *expr, double to_rm) {
  sm_expr *result = sm_new_expr_n(expr->op, 0, expr->size, NULL);
  for (uint32_t i = 0; i < expr->size; i++) {
    sm_object *current_obj = sm_expr_get_arg(expr, i);
    if (current_obj->my_type == SM_F64_TYPE) {
      if (((sm_f64 *)current_obj)->value != to_rm) {
        result = sm_expr_append(result, current_obj);
      }
    } else
      result = sm_expr_append(result, current_obj);
  }
  return result;
}

// Return 0 if it's a times_expr and has_num(0)
sm_expr *apply_constants0(sm_expr *e) {
  if (e->my_type == SM_EXPR_TYPE) {
    if (e->op == SM_TIMES_EXPR && sm_expr_has_num(e, 0))
      return (sm_expr *)sm_new_f64(0);
    else {
      sm_expr *new_expr = sm_new_expr_n(e->op, 0, e->size, NULL);
      for (uint32_t i = 0; i < e->size; i++) {
        sm_expr *current_obj = (sm_expr *)sm_expr_get_arg(e, i);
        sm_expr_append(new_expr, (sm_object *)apply_constants0(current_obj));
      }
      return new_expr;
    }
  } else
    return e;
}

// Rm empties and singles
sm_expr *apply_constants1(sm_expr *e) {
  if (e->my_type != SM_EXPR_TYPE) {
    return e;
  }
  if (e->size == 1 && sm_is_infix(e->op)) {
    return (sm_expr *)sm_expr_get_arg(e, 0);
  }
  if (e->size == 0) {
    if (e->op == SM_TIMES_EXPR) {
      return (sm_expr *)sm_new_f64(1);
    } else if (e->op == SM_PLUS_EXPR) {
      return (sm_expr *)sm_new_f64(0);
    }
  }
  sm_expr *new_e = sm_new_expr_n(e->op, 0, e->size, NULL);
  for (uint32_t i = 0; i < e->size; i++) {
    sm_object *obj = sm_expr_get_arg(e, i);
    if (obj->my_type == SM_EXPR_TYPE) {
      sm_expr *sub_e = apply_constants1((sm_expr *)obj);
      sm_expr_append(new_e, (sm_object *)sub_e);
    } else
      new_e = sm_expr_append(new_e, obj);
  }
  return new_e;
}

// Return 1 of it's an empty times_expr
sm_expr *apply_constants2(sm_expr *e) {
  if (e->my_type == SM_EXPR_TYPE) {
    if (e->op == SM_TIMES_EXPR && e->size == 0)
      return (sm_expr *)sm_new_f64(1);
    else {
      sm_expr *new_expr = sm_new_expr_n(e->op, 0, e->size, NULL);
      for (uint32_t i = 0; i < e->size; i++) {
        sm_expr *current_obj = (sm_expr *)sm_expr_get_arg(e, i);
        sm_expr_append(new_expr, (sm_object *)apply_constants2(current_obj));
      }
      return new_expr;
    }
  } else
    return e;
}

// 0 / anything is 0
sm_expr *apply_constants3(sm_expr *e) {
  if (e->my_type == SM_EXPR_TYPE) {
    if (e->op == SM_DIVIDE_EXPR && e->size == 2) {
      sm_object *obj0 = sm_expr_get_arg(e, 0);
      if (obj0->my_type == SM_F64_TYPE) {
        if (((sm_f64 *)obj0)->value == 0) {
          return (sm_expr *)sm_new_f64(0);
        }
      }
    }
    sm_expr *new_expr = sm_new_expr_n(e->op, 0, e->size, NULL);
    for (uint32_t i = 0; i < e->size; i++) {
      sm_expr *current_obj = (sm_expr *)sm_expr_get_arg(e, i);
      sm_expr_append(new_expr, (sm_object *)apply_constants3(current_obj));
    }
    return new_expr;
  } else
    return e;
}

// 0 - anything is -anything
sm_expr *apply_constants4(sm_expr *e) {
  if (e->my_type == SM_EXPR_TYPE) {
    if (e->op == SM_MINUS_EXPR && e->size == 2) {
      sm_object *obj0 = sm_expr_get_arg(e, 0);
      sm_object *obj1 = sm_expr_get_arg(e, 1);
      if (obj0->my_type == SM_F64_TYPE) {
        if (((sm_f64 *)obj0)->value == 0) {
          if (obj1->my_type == SM_F64_TYPE) {
            return (sm_expr *)sm_new_f64(-1 * ((sm_f64 *)obj1)->value);
          } else
            return sm_new_expr_2(SM_TIMES_EXPR, (sm_object *)sm_new_f64(-1), obj1, NULL);
        }
      }
    }
    sm_expr *new_expr = sm_new_expr_n(e->op, 0, e->size, NULL);
    for (uint32_t i = 0; i < e->size; i++) {
      sm_expr *current_obj = (sm_expr *)sm_expr_get_arg(e, i);
      sm_expr_append(new_expr, (sm_object *)apply_constants4(current_obj));
    }
    return new_expr;
  } else
    return e;
}

// a * b = ab
sm_expr *apply_constants5(sm_expr *e) {
  if (e->my_type == SM_EXPR_TYPE) {
    if (e->op == SM_TIMES_EXPR && e->size == 2) {
      sm_object *obj0 = sm_expr_get_arg(e, 0);
      sm_object *obj1 = sm_expr_get_arg(e, 1);
      if (obj0->my_type == SM_F64_TYPE) {
        if (obj1->my_type == SM_F64_TYPE) {
          return (sm_expr *)sm_new_f64(((sm_f64 *)obj0)->value * ((sm_f64 *)obj1)->value);
        }
      }
    }
    sm_expr *new_expr = sm_new_expr_n(e->op, 0, e->size, NULL);
    for (uint32_t i = 0; i < e->size; i++) {
      sm_expr *current_obj = (sm_expr *)sm_expr_get_arg(e, i);
      sm_expr_append(new_expr, (sm_object *)apply_constants5(current_obj));
    }
    return new_expr;
  } else
    return e;
}

// a * (b*..) = ab * ...
sm_expr *apply_constants6(sm_expr *e) {
  if (e->my_type == SM_EXPR_TYPE) {
    if (e->op == SM_TIMES_EXPR && e->size == 2) {
      sm_object *obj0 = sm_expr_get_arg(e, 0);
      sm_object *obj1 = sm_expr_get_arg(e, 1);
      if (obj0->my_type == SM_F64_TYPE) {
        if (obj1->my_type == SM_EXPR_TYPE && ((sm_expr *)obj1)->op == SM_TIMES_EXPR) {
          sm_object *leftmost = sm_expr_get_arg((sm_expr *)obj1, 0);
          if (leftmost->my_type == SM_F64_TYPE) {
            sm_expr *result =
              (sm_expr *)sm_new_f64(((sm_f64 *)obj0)->value * ((sm_f64 *)leftmost)->value);
            sm_expr *expr1     = ((sm_expr *)obj1);
            sm_expr *remainder = sm_new_expr_n(SM_TIMES_EXPR, 0, expr1->size - 1, NULL);
            for (uint32_t i = 1; i < expr1->size; i++) {
              sm_expr_append(remainder, sm_expr_get_arg(expr1, i));
            }
            result =
              sm_new_expr_2(SM_TIMES_EXPR, (sm_object *)result, (sm_object *)remainder, NULL);
            return result;
          }
        }
      }
    }
    sm_expr *new_expr = sm_new_expr_n(e->op, 0, e->size, NULL);
    for (uint32_t i = 0; i < e->size; i++) {
      sm_expr *current_obj = (sm_expr *)sm_expr_get_arg(e, i);
      sm_expr_append(new_expr, (sm_object *)apply_constants6(current_obj));
    }
    return new_expr;
  } else
    return e;
}

// a +- b = (a+-b)
sm_expr *apply_constants7(sm_expr *e) {
  if (e->my_type == SM_EXPR_TYPE) {
    if (e->size == 2 && (e->op == SM_PLUS_EXPR || e->op == SM_MINUS_EXPR)) {
      sm_object *obj0 = sm_expr_get_arg(e, 0);
      sm_object *obj1 = sm_expr_get_arg(e, 1);
      if (obj0->my_type == SM_F64_TYPE) {
        if (obj1->my_type == SM_F64_TYPE) {
          sm_f64 *d0 = (sm_f64 *)obj0;
          sm_f64 *d1 = (sm_f64 *)obj1;
          if (e->op == SM_PLUS_EXPR)
            return (sm_expr *)sm_new_f64(d0->value + d1->value);
          else
            return (sm_expr *)sm_new_f64(d0->value - d1->value);
        }
      }
    }
    sm_expr *new_expr = sm_new_expr_n(e->op, 0, e->size, NULL);
    for (uint32_t i = 0; i < e->size; i++) {
      sm_expr *current_obj = (sm_expr *)sm_expr_get_arg(e, i);
      sm_expr_append(new_expr, (sm_object *)apply_constants7(current_obj));
    }
    return new_expr;
  } else
    return e;
}

// anything ^ 1 = anything
// anything ^ 0 = 1
sm_expr *apply_constants8(sm_expr *e) {
  if (e->my_type == SM_EXPR_TYPE) {
    if (e->op == SM_POW_EXPR) {
      sm_object *obj1 = sm_expr_get_arg(e, 1);
      if (obj1->my_type == SM_F64_TYPE) {
        sm_f64 *d1 = (sm_f64 *)obj1;
        if (d1->value == 1)
          return (sm_expr *)sm_expr_get_arg(e, 0);
        else if (d1->value == 0)
          return (sm_expr *)sm_new_f64(1);
      }
    }
    sm_expr *new_expr = sm_new_expr_n(e->op, 0, e->size, NULL);
    for (uint32_t i = 0; i < e->size; i++) {
      sm_expr *current_obj = (sm_expr *)sm_expr_get_arg(e, i);
      sm_expr_append(new_expr, (sm_object *)apply_constants8(current_obj));
    }
    return new_expr;
  } else
    return e;
}

// Skip 0's in plus, skip 1's in times
sm_expr *apply_constants9(sm_expr *self) {
  if (self->my_type != SM_EXPR_TYPE) {
    return self;
  }
  sm_expr *orig_expr = self;
  sm_expr *new_expr  = sm_new_expr_n(orig_expr->op, 0, orig_expr->size, NULL);
  new_expr->op       = orig_expr->op;
  for (uint32_t i = 0; i < orig_expr->size; i++) {
    sm_object *arg = sm_expr_get_arg(orig_expr, i);
    if (arg->my_type == SM_F64_TYPE) {
      sm_f64 *num = (sm_f64 *)arg;
      if (num->value == 0.0) {
        if (orig_expr->op == SM_PLUS_EXPR || (orig_expr->op == SM_MINUS_EXPR && i > 0)) {
          continue;
        }
      }
      if (num->value == 1.0) {
        if (orig_expr->op == SM_TIMES_EXPR || (orig_expr->op == SM_DIVIDE_EXPR && i > 0)) {
          continue;
        }
      }
    }
    new_expr = sm_expr_append(new_expr, (sm_object *)apply_constants9((sm_expr *)arg));
  }
  sm_expr *result = new_expr;
  return result;
}

// const ^ const = const
sm_expr *apply_constants10(sm_expr *e) {
  if (e->my_type == SM_EXPR_TYPE) {
    if (e->op == SM_POW_EXPR) {
      sm_object *obj0 = sm_expr_get_arg(e, 0);
      sm_object *obj1 = sm_expr_get_arg(e, 1);
      if (obj0->my_type == SM_F64_TYPE) {
        sm_f64 *d0 = (sm_f64 *)obj0;
        if (obj1->my_type == SM_F64_TYPE) {
          sm_f64 *d1 = (sm_f64 *)obj1;
          return (sm_expr *)sm_new_f64(pow(d0->value, d1->value));
        }
      }
    }
    sm_expr *new_expr = sm_new_expr_n(e->op, 0, e->size, NULL);
    for (uint32_t i = 0; i < e->size; i++) {
      sm_expr *current_obj = (sm_expr *)sm_expr_get_arg(e, i);
      sm_expr_append(new_expr, (sm_object *)apply_constants10(current_obj));
    }
    return new_expr;
  } else
    return e;
}

// a / b , check if a%b==0, if so, simplify
int gcd(int a, int b) {
  int temp;
  while (b != 0) {
    temp = a % b;

    a = b;
    b = temp;
  }
  return a;
}
sm_expr *apply_constants11(sm_expr *e) {
  if (e->my_type == SM_EXPR_TYPE) {
    if (e->op == SM_DIVIDE_EXPR) {
      sm_object *obj0 = sm_expr_get_arg(e, 0);
      sm_object *obj1 = sm_expr_get_arg(e, 1);
      if (obj0->my_type == SM_F64_TYPE && obj1->my_type == SM_F64_TYPE) {
        sm_f64 *d0 = (sm_f64 *)obj0;
        sm_f64 *d1 = (sm_f64 *)obj1;
        double  i0 = d0->value;
        double  i1 = d1->value;

        if (i1 != 0 && (int)i0 == i0 && (int)i1 == i1) { // Check if both values are integers
          int int0 = (int)i0;
          int int1 = (int)i1;

          uint32_t divisor        = gcd(abs(int0), abs(int1));
          int      simplified_num = int0 / divisor;
          int      simplified_den = int1 / divisor;

          if (simplified_den ==
              1) { // If the denominator is 1, return the numerator as a single number
            return (sm_expr *)((sm_object *)sm_new_f64(simplified_num));
          } else { // Otherwise, return the simplified fraction
            return (sm_expr *)sm_new_expr_2(SM_DIVIDE_EXPR, (sm_object *)sm_new_f64(simplified_num),
                                            (sm_object *)sm_new_f64(simplified_den), NULL);
          }
        }
        return (sm_expr *)sm_new_expr_2(SM_DIVIDE_EXPR, (sm_object *)sm_new_f64(i0),
                                        (sm_object *)sm_new_f64(i1), NULL);
      }
    }
    sm_expr *new_expr = sm_new_expr_n(e->op, 0, e->size, NULL);
    for (uint32_t i = 0; i < e->size; i++) {
      sm_expr *current_obj = (sm_expr *)sm_expr_get_arg(e, i);
      sm_expr_append(new_expr, (sm_object *)apply_constants11(current_obj));
    }
    return new_expr;
  }
  return e;
}


// a ^ (b ^ c) = a ^ bc
sm_expr *apply_constants12(sm_expr *e) {
  if (e->my_type == SM_EXPR_TYPE) {
    if (e->op == SM_POW_EXPR) {
      sm_object *obj0 = sm_expr_get_arg(e, 0);
      sm_object *obj1 = sm_expr_get_arg(e, 1);
      if (obj1->my_type == SM_EXPR_TYPE && ((sm_expr *)obj1)->op == SM_POW_EXPR) {
        sm_expr   *right_pow   = (sm_expr *)obj1;
        sm_object *objb        = sm_expr_get_arg(right_pow, 0);
        sm_object *objc        = sm_expr_get_arg(right_pow, 1);
        sm_expr   *new_product = sm_new_expr_2(SM_TIMES_EXPR, objb, objc, NULL);
        sm_expr   *new_expr    = sm_new_expr_2(SM_POW_EXPR, obj0, (sm_object *)new_product, NULL);
        return new_expr;
      }
    }
    sm_expr *new_expr = sm_new_expr_n(e->op, 0, e->size, NULL);
    for (uint32_t i = 0; i < e->size; i++) {
      sm_expr *current_obj = (sm_expr *)sm_expr_get_arg(e, i);
      sm_expr_append(new_expr, (sm_object *)apply_constants12(current_obj));
    }
    return new_expr;
  } else
    return e;
}

// (a ^ b) ^ c = a ^ bc
sm_expr *apply_constants13(sm_expr *e) {
  if (e->my_type == SM_EXPR_TYPE) {
    if (e->op == SM_POW_EXPR) {
      sm_object *obj0 = sm_expr_get_arg(e, 0);
      sm_object *objc = sm_expr_get_arg(e, 1);
      if (obj0->my_type == SM_EXPR_TYPE && ((sm_expr *)obj0)->op == SM_POW_EXPR) {
        sm_object *objb        = sm_expr_get_arg((sm_expr *)obj0, 1);
        sm_object *obja        = sm_expr_get_arg((sm_expr *)obj0, 0);
        sm_expr   *new_product = sm_new_expr_2(SM_TIMES_EXPR, objb, objc, NULL);
        sm_expr   *new_expr    = sm_new_expr_2(SM_POW_EXPR, obja, (sm_object *)new_product, NULL);
        return new_expr;
      }
    }
    sm_expr *new_expr = sm_new_expr_n(e->op, 0, e->size, NULL);
    for (uint32_t i = 0; i < e->size; i++) {
      sm_expr *current_obj = (sm_expr *)sm_expr_get_arg(e, i);
      sm_expr_append(new_expr, (sm_object *)apply_constants13(current_obj));
    }
    return new_expr;
  } else
    return e;
}

// a / b , check if sm_object_eq(a,b), if so, return 1
sm_expr *apply_constants14(sm_expr *e) {
  if (e->my_type == SM_EXPR_TYPE) {
    if (e->op == SM_DIVIDE_EXPR) {
      sm_object *obj0 = sm_expr_get_arg(e, 0);
      sm_object *obj1 = sm_expr_get_arg(e, 1);
      if (sm_object_eq(obj0, obj1)) {
        return (sm_expr *)((sm_object *)sm_new_f64(1));
      }
    }
    sm_expr *new_expr = sm_new_expr_n(e->op, 0, e->size, NULL);
    for (uint32_t i = 0; i < e->size; i++) {
      sm_expr *current_obj = (sm_expr *)sm_expr_get_arg(e, i);
      sm_expr_append(new_expr, (sm_object *)apply_constants14(current_obj));
    }
    return new_expr;
  } else
    return e;
}

// Return the first element (starting at pos) NOT of this type or -1
uint32_t find_next_not(sm_expr *e, uint32_t pos, uint16_t t) {
  for (uint32_t i = pos + 1; i < e->size; i++) {
    sm_object *current_obj = sm_expr_get_arg(e, i);
    if (current_obj->my_type != t) {
      return i;
    }
  }
  return -1;
}

// +(...) or *(...) put all constants on the left and apply
sm_expr *apply_constants15(sm_expr *e) {
  if (e->my_type == SM_EXPR_TYPE) {
    sm_expr *new_expr = sm_new_expr_n(e->op, 0, e->size, NULL);
    if (e->op == SM_PLUS_EXPR || e->op == SM_TIMES_EXPR) {
      uint32_t f64_locations[e->size];
      uint32_t num_count = 0;
      for (uint32_t j = 0; j < e->size; j++) {
        sm_object *current_obj = sm_expr_get_arg(e, j);
        if (current_obj->my_type == SM_F64_TYPE) {
          f64_locations[num_count++] = j;
        }
      }
      // put numbers first, unified
      if (num_count > 0) {
        uint32_t num_index;
        double   calculation = 0;
        if (e->op == SM_TIMES_EXPR)
          calculation = 1;
        for (num_index = 0; num_index < num_count; num_index++) {
          sm_f64 *current_num = (sm_f64 *)sm_expr_get_arg(e, f64_locations[num_index]);
          if (e->op == SM_PLUS_EXPR)
            calculation += current_num->value;
          else
            calculation *= current_num->value;
        }
        sm_expr_append(new_expr, (sm_object *)sm_new_f64(calculation));
      }
      // then add the rest
      uint32_t next_not = -1;
      for (uint32_t last_i = num_count; last_i < e->size; last_i++) {
        next_not = find_next_not(e, next_not, SM_F64_TYPE);
        sm_expr_append(new_expr, sm_expr_get_arg(e, next_not));
      }
      return new_expr;
    }
    for (uint32_t i = 0; i < e->size; i++) {
      sm_expr *current_obj = (sm_expr *)sm_expr_get_arg(e, i);
      sm_expr_append(new_expr, (sm_object *)apply_constants15(current_obj));
    }
    return new_expr;
  }
  return e;
}

// x - x = 0
sm_expr *apply_constants16(sm_expr *e) {
  if (e->my_type == SM_EXPR_TYPE) {
    if (e->op == SM_MINUS_EXPR && e->size == 2) {
      sm_object *obj0 = sm_expr_get_arg(e, 0);
      sm_object *obj1 = sm_expr_get_arg(e, 1);
      if (sm_object_eq(obj0, obj1)) {
        return (sm_expr *)sm_new_f64(0);
      }
    }
    sm_expr *new_expr = sm_new_expr_n(e->op, 0, e->size, NULL);
    for (uint32_t i = 0; i < e->size; i++) {
      sm_expr *current_obj = (sm_expr *)sm_expr_get_arg(e, i);
      sm_expr_append(new_expr, (sm_object *)apply_constants16(current_obj));
    }
    return new_expr;
  } else
    return e;
}


sm_expr *apply_constants17(sm_expr *e) {
  if (e->my_type == SM_EXPR_TYPE) {
    sm_expr *new_expr = sm_new_expr_n(e->op, 0, e->size, NULL);

    if (e->op == SM_TIMES_EXPR) {
      int    count_numbers = 0;
      double total         = 1;
      int    non_one_terms = 0;

      for (uint32_t i = 0; i < e->size; i++) {
        sm_object *current_obj = sm_expr_get_arg(e, i);
        if (current_obj->my_type == SM_F64_TYPE) {
          sm_f64 *current_f64 = (sm_f64 *)current_obj;
          if (current_f64->value == 1) {
            continue; // Skip multiplying by 1
          }
          total *= current_f64->value;
          count_numbers++;
        } else {
          new_expr =
            sm_expr_append(new_expr, (sm_object *)apply_constants17((sm_expr *)current_obj));
          non_one_terms++;
        }
      }

      if (count_numbers > 0 && total != 1) {
        new_expr = sm_expr_append(new_expr, (sm_object *)sm_new_f64(total));
      }

      // If new_expr is empty, it means we had only 1s
      if (new_expr->size == 0) {
        return (sm_expr *)sm_new_f64(1);
      }

      // If we have exactly one non-one term and no numbers other than 1, return the term itself
      if (non_one_terms == 1 && count_numbers == 0) {
        return (sm_expr *)sm_expr_get_arg(new_expr, 0);
      }

      return new_expr;
    }

    for (uint32_t i = 0; i < e->size; i++) {
      sm_expr *current_obj = (sm_expr *)sm_expr_get_arg(e, i);
      new_expr             = sm_expr_append(new_expr, (sm_object *)apply_constants17(current_obj));
    }

    return new_expr;
  }
  return e;
}
// Run each simplifier until the expression stops changing.
sm_object *sm_simplify(sm_object *obj) {
  sm_expr *result      = (sm_expr *)obj;
  sm_expr *last_result = result;
  do {
    last_result = result;
    result      = apply_constants0(result);
    result      = apply_constants1(result);
    result      = apply_constants2(result);
    result      = apply_constants3(result);
    result      = apply_constants4(result);
    result      = apply_constants5(result);
    result      = apply_constants6(result);
    result      = apply_constants7(result);
    result      = apply_constants8(result);
    result      = apply_constants9(result);
    result      = apply_constants10(result);
    result      = apply_constants11(result);
    result      = apply_constants12(result);
    result      = apply_constants13(result);
    result      = apply_constants14(result);
    result      = apply_constants15(result);
    result      = apply_constants16(result);
    result      = apply_constants17(result);
  } while (sm_object_eq((sm_object *)last_result, (sm_object *)result) == false);
  return (sm_object *)result;
}
