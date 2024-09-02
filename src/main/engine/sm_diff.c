// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"

// Return whether these two symbols have the same name.
bool sm_symbol_equal(sm_symbol *s1, sm_symbol *s2) {
  return !strncmp(&s1->name->content, &s2->name->content, MIN(s1->name->size, s2->name->size));
}

// return true only if the expression tree 'self' contains the symbol sym
bool has_symbol(sm_expr *self, sm_symbol *sym) {
  switch (self->my_type) {
  case SM_EXPR_TYPE: {
    switch (((sm_expr *)self)->op) {
    case SM_PLUS_EXPR:
    case SM_MINUS_EXPR:
    case SM_TIMES_EXPR:
    case SM_DIVIDE_EXPR:
    case SM_POW_EXPR:
    case SM_SQRT_EXPR:
    case SM_SIN_EXPR:
    case SM_COS_EXPR:
    case SM_TAN_EXPR:
    case SM_SINH_EXPR:
    case SM_COSH_EXPR:
    case SM_TANH_EXPR:
    case SM_CSC_EXPR:
    case SM_SEC_EXPR:
    case SM_COT_EXPR:
    case SM_CSCH_EXPR:
    case SM_SECH_EXPR:
    case SM_COTH_EXPR: {
      for (uint32_t i = 0; i < self->size; i++) {
        sm_object *current_obj = sm_expr_get_arg(self, i);
        switch (current_obj->my_type) {
        case SM_SYMBOL_TYPE: {
          if (sm_symbol_equal(sym, (sm_symbol *)current_obj)) {
            return true;
          }
        }
        case SM_EXPR_TYPE: {
          sm_expr *expr = (sm_expr *)current_obj;
          if (has_symbol(expr, sym))
            return true;
        }
        }
      }
    }
    default:
      return false;
    }
  }
  case SM_SYMBOL_TYPE: {
    sm_symbol *self_sym = (sm_symbol *)self;
    if (sm_symbol_equal(self_sym, sym)) {
      return true;
    }
  default:
    return false;
  }
  }
}

// Return an expression that is the derivative of obj, with respect to wrt
// If the object is not an expression, just return the object.

sm_object *sm_diff(sm_object *obj, sm_symbol *wrt) {
  switch (obj->my_type) {
  case SM_SYMBOL_TYPE: {
    sm_symbol *sym = (sm_symbol *)obj;
    if (sm_symbol_equal(wrt, sym))
      return (sm_object *)sm_new_f64(1);
    else
      return (sm_object *)sm_new_f64(0);
  }
  case SM_F64_TYPE:
    return (sm_object *)sm_new_f64(0);
  case SM_EXPR_TYPE: {
    sm_expr *expr = (sm_expr *)obj;
    switch (expr->op) {
    case SM_DIFF_EXPR: {
      sm_object *to_diff_twice = sm_expr_get_arg((sm_expr *)obj, 0);
      return sm_diff(sm_diff(to_diff_twice, wrt), wrt);
    }
    case SM_POW_EXPR: {
      sm_object *obj0     = sm_expr_get_arg(expr, 0);
      sm_object *obj1     = sm_expr_get_arg(expr, 1);
      bool       obj0_has = has_symbol((sm_expr *)obj0, wrt);
      bool       obj1_has = has_symbol((sm_expr *)obj1, wrt);
      if (obj0_has && !obj1_has) {
        sm_object *deducted_expr =
          (sm_object *)sm_new_expr_2(SM_MINUS_EXPR, obj1, (sm_object *)sm_new_f64(1), NULL);
        sm_object *pow_expr = (sm_object *)sm_new_expr_2(SM_POW_EXPR, obj0, deducted_expr, NULL);
        sm_object *diff_outside =
          (sm_object *)sm_new_expr_2(SM_TIMES_EXPR, sm_expr_get_arg(expr, 1), pow_expr, NULL);
        sm_object *diff_inside = sm_diff(sm_expr_get_arg(expr, 0), wrt);
        return (sm_object *)sm_new_expr_2(SM_TIMES_EXPR, diff_outside, diff_inside, NULL);
      } else if (!obj0_has && obj1_has) {
        sm_object *ln_expr  = (sm_object *)sm_new_expr(SM_LN_EXPR, obj0, NULL);
        sm_object *pow_expr = (sm_object *)sm_new_expr_2(SM_POW_EXPR, obj0, obj1, NULL);
        sm_object *diff_outside =
          (sm_object *)sm_new_expr_2(SM_TIMES_EXPR, ln_expr, pow_expr, NULL);
        sm_object *diff_inside = sm_diff(sm_expr_get_arg(expr, 0), wrt);
        return (sm_object *)sm_new_expr_2(SM_TIMES_EXPR, diff_outside, diff_inside, NULL);
      } else if (obj0_has && obj1_has) {
        sm_expr *p1 = sm_new_expr_2(SM_POW_EXPR, obj0, (sm_object *)wrt, NULL);
        sm_expr *p2 = sm_new_expr_2(SM_DIVIDE_EXPR, (sm_object *)wrt, obj0, NULL);
        sm_expr *p3 = sm_new_expr(SM_LN_EXPR, obj0, NULL);
        sm_expr *p4 = sm_new_expr_2(SM_PLUS_EXPR, (sm_object *)p2, (sm_object *)p3, NULL);
        sm_expr *p5 = sm_new_expr_2(SM_TIMES_EXPR, (sm_object *)p1, (sm_object *)p4, NULL);
        return (sm_object *)p5;
      }
      return (sm_object *)sm_new_f64(0);
    }
    case SM_PLUS_EXPR:
    case SM_MINUS_EXPR: {
      sm_expr *output = sm_new_expr_n(expr->op, expr->size, expr->size, NULL);
      for (uint32_t i = 0; i < expr->size; i++) {
        output = sm_expr_set_arg(output, i, sm_diff(sm_expr_get_arg(expr, i), wrt));
      }
      return (sm_object *)output;
    }
    case SM_TIMES_EXPR: {
      sm_expr *sum = sm_new_expr_n(SM_PLUS_EXPR, 0, expr->size, NULL);
      for (uint32_t i = 0; i < expr->size; i++) {
        sm_expr *product = sm_new_expr_n(SM_TIMES_EXPR, 0, expr->size, NULL);
        for (uint32_t j = 0; j < expr->size; j++) {
          sm_object *current_obj = sm_expr_get_arg(expr, j);
          if (j == i) {
            sm_object *diff = sm_diff(current_obj, wrt);
            product         = sm_expr_append(product, diff);
          } else
            product = sm_expr_append(product, current_obj);
        }
        if (!sm_expr_has_num(expr, 0))
          sum = sm_expr_append(sum, (sm_object *)product);
      }
      if (sum->size == 0)
        return (sm_object *)sm_new_f64(0);
      return (sm_object *)sum;
    }
    case SM_DIVIDE_EXPR: {
      sm_expr *output   = sm_new_expr_n(SM_DIVIDE_EXPR, 2, 2, NULL);
      sm_expr *aprime_b = sm_new_expr_2(SM_TIMES_EXPR, sm_diff(sm_expr_get_arg(expr, 0), wrt),
                                        sm_expr_get_arg(expr, 1), NULL);
      sm_expr *bprime_a = sm_new_expr_2(SM_TIMES_EXPR, sm_diff(sm_expr_get_arg(expr, 1), wrt),
                                        sm_expr_get_arg(expr, 0), NULL);
      sm_expr_set_arg(output, 0,
                      (sm_object *)sm_new_expr_2(SM_MINUS_EXPR, (sm_object *)aprime_b,
                                                 (sm_object *)bprime_a, NULL));
      sm_expr_set_arg(output, 1,
                      (sm_object *)sm_new_expr_2(SM_POW_EXPR, (sm_object *)sm_expr_get_arg(expr, 1),
                                                 (sm_object *)sm_new_f64(2), NULL));
      return (sm_object *)output;
    }
    case SM_SQRT_EXPR: {
      sm_object *arg = sm_expr_get_arg(expr, 0);
      sm_object *power_expr =
        (sm_object *)sm_new_expr_2(SM_POW_EXPR, arg, (sm_object *)sm_new_f64(0.5), NULL);
      sm_object *diff_power = sm_diff(power_expr, wrt);
      return (sm_object *)diff_power;
    }
    case SM_SIN_EXPR: {
      if (has_symbol(expr, wrt)) {
        sm_object *diff_outside =
          (sm_object *)sm_new_expr(SM_COS_EXPR, sm_expr_get_arg(expr, 0), NULL);
        sm_object *diff_inside = sm_diff(sm_expr_get_arg(expr, 0), wrt);
        return (sm_object *)sm_new_expr_2(SM_TIMES_EXPR, diff_inside, diff_outside, NULL);
      } else
        return (sm_object *)sm_new_f64(0);
    }
    case SM_COS_EXPR: {
      if (has_symbol(expr, wrt)) {
        sm_object *diff_outside = (sm_object *)sm_new_expr_2(
          SM_TIMES_EXPR, (sm_object *)sm_new_expr(SM_SIN_EXPR, sm_expr_get_arg(expr, 0), NULL),
          (sm_object *)sm_new_f64(-1), NULL);
        sm_object *diff_inside = sm_diff(sm_expr_get_arg(expr, 0), wrt);
        return (sm_object *)sm_new_expr_2(SM_TIMES_EXPR, diff_inside, diff_outside, NULL);
      } else
        return (sm_object *)sm_new_f64(0);
    }
    case SM_TAN_EXPR: {
      if (has_symbol(expr, wrt)) {
        sm_object *sec = (sm_object *)sm_new_expr(SM_SEC_EXPR, sm_expr_get_arg(expr, 0), NULL);
        sm_object *sec_squared =
          (sm_object *)sm_new_expr_2(SM_POW_EXPR, sec, (sm_object *)sm_new_f64(2), NULL);
        sm_object *diff_inside = sm_diff(sm_expr_get_arg(expr, 0), wrt);
        return (sm_object *)sm_new_expr_2(SM_TIMES_EXPR, diff_inside, sec_squared, NULL);
      } else
        return (sm_object *)sm_new_f64(0);
    }
    case SM_SEC_EXPR: {
      if (has_symbol(expr, wrt)) {
        sm_object *diff_outside =
          (sm_object *)sm_new_expr(SM_TAN_EXPR, sm_expr_get_arg(expr, 0), NULL);
        sm_object *diff_inside = sm_diff(sm_expr_get_arg(expr, 0), wrt);
        return (sm_object *)sm_new_expr_2(SM_TIMES_EXPR, diff_outside, diff_inside, NULL);
      } else
        return (sm_object *)sm_new_f64(0);
    }
    case SM_CSC_EXPR: {
      if (has_symbol(expr, wrt)) {
        sm_object *diff_outside = (sm_object *)sm_new_expr_3(
          SM_TIMES_EXPR, (sm_object *)sm_new_expr(SM_CSC_EXPR, sm_expr_get_arg(expr, 0), NULL),
          (sm_object *)sm_new_expr(SM_COT_EXPR, sm_expr_get_arg(expr, 0), NULL),
          (sm_object *)sm_new_f64(-1), NULL);
        sm_object *diff_inside = sm_diff(sm_expr_get_arg(expr, 0), wrt);
        return (sm_object *)sm_new_expr_2(SM_TIMES_EXPR, diff_inside, diff_outside, NULL);
      } else
        return (sm_object *)sm_new_f64(0);
    }
    case SM_COT_EXPR: {
      if (has_symbol(expr, wrt)) {
        sm_object *csc_sq = (sm_object *)sm_new_expr_2(
          SM_POW_EXPR, (sm_object *)sm_new_expr(SM_CSC_EXPR, sm_expr_get_arg(expr, 0), NULL),
          (sm_object *)sm_new_f64(2), NULL);
        sm_object *diff_outside =
          (sm_object *)sm_new_expr_2(SM_TIMES_EXPR, (sm_object *)sm_new_f64(-1), csc_sq, NULL);
        sm_object *diff_inside = sm_diff(sm_expr_get_arg(expr, 0), wrt);
        return (sm_object *)sm_new_expr_2(SM_TIMES_EXPR, diff_inside, diff_outside, NULL);
      } else
        return (sm_object *)sm_new_f64(0);
    }
    case SM_SECH_EXPR: {
      if (has_symbol(expr, wrt)) {
        sm_object *diff_outside = (sm_object *)sm_new_expr_3(
          SM_TIMES_EXPR, (sm_object *)sm_new_f64(-1),
          (sm_object *)sm_new_expr(SM_TANH_EXPR, sm_expr_get_arg(expr, 0), NULL),
          (sm_object *)sm_new_expr(SM_SECH_EXPR, sm_expr_get_arg(expr, 0), NULL), NULL);
        sm_object *diff_inside = sm_diff(sm_expr_get_arg(expr, 0), wrt);
        return (sm_object *)sm_new_expr_2(SM_TIMES_EXPR, diff_inside, diff_outside, NULL);
      } else
        return (sm_object *)sm_new_f64(0);
    }
    case SM_CSCH_EXPR: {
      if (has_symbol(expr, wrt)) {
        sm_object *diff_outside = (sm_object *)sm_new_expr_3(
          SM_TIMES_EXPR, (sm_object *)sm_new_f64(-1),
          (sm_object *)sm_new_expr(SM_COTH_EXPR, sm_expr_get_arg(expr, 0), NULL),
          (sm_object *)sm_new_expr(SM_CSCH_EXPR, sm_expr_get_arg(expr, 0), NULL), NULL);
        sm_object *diff_inside = sm_diff(sm_expr_get_arg(expr, 0), wrt);
        return (sm_object *)sm_new_expr_2(SM_TIMES_EXPR, diff_inside, diff_outside, NULL);
      } else
        return (sm_object *)sm_new_f64(0);
    }
    case SM_COTH_EXPR: {
      if (has_symbol(expr, wrt)) {
        sm_object *csch_sq = (sm_object *)sm_new_expr_2(
          SM_POW_EXPR, (sm_object *)sm_new_expr(SM_CSCH_EXPR, sm_expr_get_arg(expr, 0), NULL),
          (sm_object *)sm_new_f64(2), NULL);
        sm_object *diff_outside =
          (sm_object *)sm_new_expr_2(SM_TIMES_EXPR, (sm_object *)sm_new_f64(-1), csch_sq, NULL);
        sm_object *diff_inside = sm_diff(sm_expr_get_arg(expr, 0), wrt);
        return (sm_object *)sm_new_expr_2(SM_TIMES_EXPR, diff_inside, diff_outside, NULL);
      } else
        return (sm_object *)sm_new_f64(0);
    }
    case SM_SINH_EXPR: {
      if (has_symbol(expr, wrt)) {
        sm_object *diff_outside =
          (sm_object *)sm_new_expr(SM_COSH_EXPR, sm_expr_get_arg(expr, 0), NULL);
        sm_object *diff_inside = sm_diff(sm_expr_get_arg(expr, 0), wrt);
        return (sm_object *)sm_new_expr_2(SM_TIMES_EXPR, diff_inside, diff_outside, NULL);
      } else
        return (sm_object *)sm_new_f64(0);
    }
    case SM_COSH_EXPR: {
      if (has_symbol(expr, wrt)) {
        sm_object *diff_outside = (sm_object *)sm_new_expr_2(
          SM_TIMES_EXPR, (sm_object *)sm_new_expr(SM_SINH_EXPR, sm_expr_get_arg(expr, 0), NULL),
          (sm_object *)sm_new_f64(-1), NULL);
        sm_object *diff_inside = sm_diff(sm_expr_get_arg(expr, 0), wrt);
        return (sm_object *)sm_new_expr_2(SM_TIMES_EXPR, diff_inside, diff_outside, NULL);
      } else
        return (sm_object *)sm_new_f64(0);
    }
    case SM_TANH_EXPR: {
      if (has_symbol(expr, wrt)) {
        sm_object *sech = (sm_object *)sm_new_expr(SM_SECH_EXPR, sm_expr_get_arg(expr, 0), NULL);
        sm_object *sec_squared =
          (sm_object *)sm_new_expr_2(SM_POW_EXPR, sech, (sm_object *)sm_new_f64(2), NULL);
        sm_object *diff_inside = sm_diff(sm_expr_get_arg(expr, 0), wrt);
        return (sm_object *)sm_new_expr_2(SM_TIMES_EXPR, diff_inside, sec_squared, NULL);
      } else
        return (sm_object *)sm_new_f64(0);
    }
    default: {
      return (sm_object *)sm_new_f64(0);
    }
    }
  }
  default: {
    return (sm_object *)sm_new_f64(0);
  }
  }
}
