// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "sms.h"

sm_object *sm_diff(sm_object *obj) {
  switch (obj->my_type) {
  case sm_double_type:
    return (sm_object *)sm_new_double(0);
  case sm_expr_type: {
    sm_expr *expr = (sm_expr *)obj;
    switch (expr->op) {
    case sm_diff_expr: {
      sm_object *to_diff_twice = sm_expr_get_arg((sm_expr *)obj, 0);
      return sm_diff(sm_diff(to_diff_twice));
    }
    case sm_minus_expr:
    case sm_plus_expr: {
      sm_expr *output = sm_new_expr_n(expr->op, expr->size, expr->size);
      for (unsigned int i = 0; i < expr->size; i++) {
        output = sm_expr_set_arg(output, i, sm_diff(sm_expr_get_arg(expr, i)));
      }
      return (sm_object *)output;
    }
    case sm_times_expr: {
      unsigned int i   = 0;
      sm_expr     *sum = sm_new_expr_n(sm_plus_expr, expr->size, expr->size);
      do {
        sm_expr *product = sm_new_expr_n(sm_times_expr, expr->size, expr->size);
        for (unsigned int j = 0; j < expr->size; j++) {
          sm_object *current_obj = sm_expr_get_arg(expr, j);
          if (j == i)
            product = sm_expr_set_arg(product, j, sm_diff(current_obj));
          else
            product = sm_expr_set_arg(product, j, current_obj);
        }
        sum = sm_expr_set_arg(sum, i, (sm_object *)product);
      } while (++i < expr->size);
      return (sm_object *)sum;
    }
    case sm_divide_expr: {
      // input is a/b
      sm_expr *output = sm_new_expr_n(sm_divide_expr, 2, 2);
      sm_expr *aprime_b =
        sm_new_expr_2(sm_times_expr, sm_diff(sm_expr_get_arg(expr, 0)), sm_expr_get_arg(expr, 1));
      sm_expr *bprime_a =
        sm_new_expr_2(sm_times_expr, sm_diff(sm_expr_get_arg(expr, 1)), sm_expr_get_arg(expr, 0));
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
      return (sm_object *)sm_new_expr(sm_cos_expr, sm_expr_get_arg(expr, 0));
    }
    case sm_cos_expr: {
      return (sm_object *)sm_new_expr_2(
        sm_times_expr, (sm_object *)sm_new_double(-1),
        (sm_object *)sm_new_expr(sm_sin_expr, sm_expr_get_arg(expr, 0)));
    }
    case sm_tan_expr: {
      return (sm_object *)sm_new_expr_2(
        sm_pow_expr, (sm_object *)sm_new_expr(sm_sec_expr, sm_expr_get_arg(expr, 0)),
        (sm_object *)sm_new_double(2));
    }
    case sm_sinh_expr: {
      return (sm_object *)sm_new_expr(sm_cosh_expr, sm_expr_get_arg(expr, 0));
    }
    case sm_cosh_expr: {
      return (sm_object *)sm_new_expr_2(
        sm_times_expr, (sm_object *)sm_new_double(-1),
        (sm_object *)sm_new_expr(sm_sinh_expr, sm_expr_get_arg(expr, 0)));
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
