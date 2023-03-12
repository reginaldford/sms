// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "sms.h"

bool sm_symbol_equal(sm_symbol *s1, sm_symbol *s2) {
  return !strcmp(&s1->name->content, &s2->name->content);
}

// distribute
// group like terms
// apply constant arith
// x+0=x
// x-0=x
// x*1=x
// x/1=x
// x^1=x
// *x^-1=/x
bool has_symbol(sm_expr *self, sm_symbol *sym) {
  switch (self->my_type) {
  case sm_expr_type: {
    switch (((sm_expr *)self)->op) {
    case sm_sqrt_expr:
    case sm_minus_expr:
    case sm_pow_expr:
    case sm_sin_expr:
    case sm_cos_expr:
    case sm_tan_expr:
    case sm_sinh_expr:
    case sm_cosh_expr:
    case sm_tanh_expr:
    case sm_csc_expr:
    case sm_sec_expr:
    case sm_cot_expr:
    case sm_times_expr:
    case sm_divide_expr:
    case sm_plus_expr: {
      for (unsigned int i = 0; i < self->size; i++) {
        sm_object *current_obj = sm_expr_get_arg(self, i);
        switch (current_obj->my_type) {
        case sm_symbol_type: {
          if (sm_symbol_equal(sym, (sm_symbol *)current_obj)) {
            return true;
          }
        }
        case sm_expr_type: {
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
  default:
    return false;
  }
  }
}

sm_object *sm_diff(sm_object *obj, sm_symbol *wrt) {
  switch (obj->my_type) {
  case sm_symbol_type: {
    sm_symbol *sym = (sm_symbol *)obj;
    if (sm_symbol_equal(wrt, sym))
      return (sm_object *)sm_new_double(1);
    else
      return (sm_object *)sm_new_double(0);
  }
  case sm_double_type:
    return (sm_object *)sm_new_double(0);
  case sm_expr_type: {
    // The rest of the cases are expr_type
    sm_expr *expr = (sm_expr *)obj;
    switch (expr->op) {
    case sm_diff_expr: {
      sm_object *to_diff_twice = sm_expr_get_arg((sm_expr *)obj, 0);
      return sm_diff(sm_diff(to_diff_twice, wrt), wrt);
    }
    case sm_pow_expr: {
      if (has_symbol(expr, wrt)) {
        sm_object *obj0 = sm_expr_get_arg(expr, 0);
        sm_object *obj1 = sm_expr_get_arg(expr, 1);
        //!! figure out it it's x^y or y^x, return accordingly
        //!! or maybe it's both?
        // if(has_symbol((sm_expr*)obj0,wrt)==true && has_symbol((sm_expr*)obj1,wrt)==false){
        sm_object *deducted_expr =
          (sm_object *)sm_new_expr_2(sm_minus_expr, obj1, (sm_object *)sm_new_double(1));
        return (sm_object *)sm_new_expr_2(
          sm_times_expr, obj1, (sm_object *)sm_new_expr_2(sm_pow_expr, obj0, deducted_expr));
        // }
      }
      return (sm_object *)sm_new_double(0);
    }
    case sm_plus_expr:
    case sm_minus_expr: {
      sm_expr *output = sm_new_expr_n(expr->op, expr->size, expr->size);
      for (unsigned int i = 0; i < expr->size; i++) {
        output = sm_expr_set_arg(output, i, sm_diff(sm_expr_get_arg(expr, i), wrt));
      }
      return (sm_object *)output;
    }
    case sm_times_expr: {
      sm_expr *sum = sm_new_expr_n(sm_plus_expr, 0, expr->size);
      for (unsigned int i = 0; i < expr->size; i++) {
        sm_expr *product = sm_new_expr_n(sm_times_expr, 0, expr->size);
        for (unsigned int j = 0; j < expr->size; j++) {
          sm_object *current_obj = sm_expr_get_arg(expr, j);
          if (j == i) {
            sm_object *diff = sm_diff(current_obj, wrt);
            sm_expr_append(product, diff);
          } else
            sm_expr_append(product, current_obj);
        }
        if (!sm_expr_has_num(expr, 0))
          sum = sm_expr_append(sum, (sm_object *)product);
      }
      if (sum->size == 0)
        return (sm_object *)sm_new_double(0);
      return (sm_object *)sum;
    }
    case sm_divide_expr: {
      // input is a/b
      sm_expr *output   = sm_new_expr_n(sm_divide_expr, 2, 2);
      sm_expr *aprime_b = sm_new_expr_2(sm_times_expr, sm_diff(sm_expr_get_arg(expr, 0), wrt),
                                        sm_expr_get_arg(expr, 1));
      sm_expr *bprime_a = sm_new_expr_2(sm_times_expr, sm_diff(sm_expr_get_arg(expr, 1), wrt),
                                        sm_expr_get_arg(expr, 0));
      sm_expr_set_arg(
        output, 0,
        (sm_object *)sm_new_expr_2(sm_minus_expr, (sm_object *)aprime_b, (sm_object *)bprime_a));
      sm_expr_set_arg(output, 1,
                      (sm_object *)sm_new_expr_2(sm_pow_expr, (sm_object *)sm_expr_get_arg(expr, 1),
                                                 (sm_object *)sm_new_double(2)));
      return (sm_object *)output;
    }
    case sm_sqrt_expr: {
      sm_object *power_val = (sm_object *)sm_new_expr_2(sm_pow_expr, sm_expr_get_arg(expr, 0),
                                                        (sm_object *)sm_new_double(-0.5));
      return (sm_object *)sm_new_expr_2(sm_times_expr, (sm_object *)sm_new_double(0.5), power_val);
    }
    case sm_sin_expr: {
      if (has_symbol(expr, wrt)) {
        sm_object *diff_outside = (sm_object *)sm_new_expr(sm_cos_expr, sm_expr_get_arg(expr, 0));
        sm_object *diff_inside  = sm_diff(sm_expr_get_arg(expr, 0), wrt);
        return (sm_object *)sm_new_expr_2(sm_times_expr, diff_inside, diff_outside);
      } else
        return (sm_object *)sm_new_double(0);
    }
    case sm_cos_expr: {
      if (has_symbol(expr, wrt)) {
        sm_object *diff_outside = (sm_object *)sm_new_expr_2(
          sm_times_expr, (sm_object *)sm_new_expr(sm_sin_expr, sm_expr_get_arg(expr, 0)),
          (sm_object *)sm_new_double(-1));
        sm_object *diff_inside = sm_diff(sm_expr_get_arg(expr, 0), wrt);
        return (sm_object *)sm_new_expr_2(sm_times_expr, diff_inside, diff_outside);
      } else
        return (sm_object *)sm_new_double(0);
    }
    case sm_tan_expr: {
      if (has_symbol(expr, wrt)) {
        sm_object *sec = (sm_object *)sm_new_expr(sm_sec_expr, sm_expr_get_arg(expr, 0));
        sm_object *sec_squared =
          (sm_object *)sm_new_expr_2(sm_pow_expr, sec, (sm_object *)sm_new_double(2));
        sm_object *diff_inside = sm_diff(sm_expr_get_arg(expr, 0), wrt);
        return (sm_object *)sm_new_expr_2(sm_times_expr, diff_inside, sec_squared);
      } else
        return (sm_object *)sm_new_double(0);
    }
    case sm_sinh_expr: {
      if (has_symbol(expr, wrt)) {
        sm_object *diff_outside = (sm_object *)sm_new_expr(sm_cosh_expr, sm_expr_get_arg(expr, 0));
        sm_object *diff_inside  = sm_diff(sm_expr_get_arg(expr, 0), wrt);
        return (sm_object *)sm_new_expr_2(sm_times_expr, diff_inside, diff_outside);
      } else
        return (sm_object *)sm_new_double(0);
    }
    case sm_cosh_expr: {
      if (has_symbol(expr, wrt)) {
        sm_object *diff_outside = (sm_object *)sm_new_expr_2(
          sm_times_expr, (sm_object *)sm_new_expr(sm_sinh_expr, sm_expr_get_arg(expr, 0)),
          (sm_object *)sm_new_double(-1));
        sm_object *diff_inside = sm_diff(sm_expr_get_arg(expr, 0), wrt);
        return (sm_object *)sm_new_expr_2(sm_times_expr, diff_inside, diff_outside);
      } else
        return (sm_object *)sm_new_double(0);
    }
    case sm_tanh_expr: {
      if (has_symbol(expr, wrt)) {
        sm_object *sech = (sm_object *)sm_new_expr(sm_sech_expr, sm_expr_get_arg(expr, 0));
        sm_object *sec_squared =
          (sm_object *)sm_new_expr_2(sm_pow_expr, sech, (sm_object *)sm_new_double(2));
        sm_object *diff_inside = sm_diff(sm_expr_get_arg(expr, 0), wrt);
        return (sm_object *)sm_new_expr_2(sm_times_expr, diff_inside, sec_squared);
      } else
        return (sm_object *)sm_new_double(0);
    }

    default: {
      return (sm_object *)sm_new_double(0);
    }
    }
  }
  default: {
    return (sm_object *)sm_new_double(0);
  }
  }
}
