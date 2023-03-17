// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "sms.h"

// Recursive engine, uses the C stack for the SMS stack
sm_object *sm_engine_eval(sm_object *input, sm_context *current_cx, sm_expr *sf) {
  switch (input->my_type) {
  case sm_expr_type: {
    sm_expr *sme = (sm_expr *)input;
    short    op  = sme->op;
    switch (op) {
    case sm_dot_expr: {
      sm_object *base_obj   = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_symbol *field_sym  = (sm_symbol *)sm_expr_get_arg(sme, 1);
      sm_string *field_name = field_sym->name;
      if (base_obj->my_type != sm_context_type) {
        sm_string *base_str = sm_object_to_string(base_obj);
        printf("Base of dot expression is not a context: %s\n", &(base_str->content));
        return (sm_object *)sm_new_double(0);
      }
      sm_context                *base_cx = (sm_context *)base_obj;
      sm_search_result_cascading sr      = sm_context_find_far(base_cx, field_name);
      if (sr.found != true) {
        sm_string *base_str = sm_object_to_string(base_obj);
        printf("Could not find variable: %s within %s\n", &(field_name->content),
               &(base_str->content));
        return (sm_object *)sm_new_double(0);
      }
      return sm_context_entries(sr.context)[sr.index].value;
    }
    case sm_diff_expr: {
      sm_object *evaluated0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_object *evaluated1 = sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      if (evaluated1->my_type != sm_symbol_type) {
        printf("Error: Second arg to diff is not a symbol: ");
        sm_string *not_sym_str = sm_object_to_string(evaluated1);
        printf("%s.\n", &(not_sym_str->content));
        return (sm_object *)sm_new_double(0);
      }
      sm_object *result = sm_diff(evaluated0, (sm_symbol *)evaluated1);
      result            = sm_simplify(result);
      return result;
    }
    case sm_simp_expr: {
      sm_object *evaluated0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      return sm_simplify(evaluated0);
    }
    case sm_fun_call_expr: {
      sm_fun         *fun      = (sm_fun *)sm_expr_get_arg(sme, 0);
      struct sm_expr *args     = (struct sm_expr *)sm_expr_get_arg(sme, 1);
      sm_expr        *new_args = (sm_expr *)sm_engine_eval((sm_object *)args, current_cx, sf);
      // When the function did not exist at fun_call creation time
      if (fun->my_type == sm_symbol_type) {
        sm_symbol *fun_sym = (sm_symbol *)fun;
        sm_object *found   = sm_context_get_by_name_far(current_cx, fun_sym->name);
        if (found == NULL) {
          printf("Error: Function not found: %s\n", &(fun_sym->name->content));
          sm_string *err_msg =
            sm_new_string(27 + fun_sym->name->size, "Error: Function not found: ");
          sm_strncpy((&err_msg->content) + 27, &fun_sym->name->content, fun_sym->name->size);
          return (sm_object *)sm_new_error(err_msg, sm_new_string(1, "."), 0);
        }
        if (found->my_type != sm_fun_type) {
          printf("Error: %s exists, but is not a function.\n", &(fun_sym->name->content));
          sm_string *err_msg = sm_new_string(47 + fun_sym->name->size,
                                             "Error: Variable exists, but is not a function: ");
          sm_strncpy((&err_msg->content) + 47, &fun_sym->name->content, fun_sym->name->size);
          return (sm_object *)sm_new_error(err_msg, sm_new_string(1, "."), 0);
        }
        sm_fun *found_fun = (sm_fun *)found;
        return sm_engine_eval(found_fun->content, found_fun->parent, new_args);
      } else if (fun->my_type == sm_fun_type) {
        return sm_engine_eval(fun->content, fun->parent, new_args);
      } else {
        return sm_engine_eval((sm_object *)fun, current_cx, new_args);
      }
    }
    case sm_then_expr: {
      sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      for (unsigned int i = 1; i < sme->size - 1; i++)
        sm_engine_eval(sm_expr_get_arg(sme, i), current_cx, sf);
      return sm_engine_eval(sm_expr_get_arg(sme, sme->size - 1), current_cx, sf);
    }
    case sm_assign_expr: {
      sm_symbol *sym   = (sm_symbol *)sm_expr_get_arg(sme, 0);
      sm_object *value = (sm_object *)sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      sm_context_set(current_cx, sym->name, value);
      return (sm_object *)sm_new_expr_2(sm_assign_expr, (sm_object *)sym, value);
    }
    case sm_plus_expr: {
      double sum = ((sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf))->value;
      for (unsigned int i = 1; i < sme->size; i++)
        sum += ((sm_double *)sm_engine_eval(sm_expr_get_arg(sme, i), current_cx, sf))->value;
      return (sm_object *)sm_new_double(sum);
    }
    case sm_minus_expr: {
      sm_object *a = sm_expr_get_arg(sme, 0);
      a            = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      double sum   = ((sm_double *)a)->value;
      for (unsigned int i = 1; i < sme->size; i++)
        sum -= ((sm_double *)sm_engine_eval(sm_expr_get_arg(sme, i), current_cx, sf))->value;
      return (sm_object *)sm_new_double(sum);
    }
    case sm_times_expr: {
      double product =
        ((sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf))->value;
      for (unsigned int i = 1; i < sme->size; i++)
        product *= ((sm_double *)sm_engine_eval(sm_expr_get_arg(sme, i), current_cx, sf))->value;
      return (sm_object *)sm_new_double(product);
    }
    case sm_divide_expr: {
      double quotient =
        ((sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf))->value;
      for (unsigned int i = 1; i < sme->size; i++)
        quotient /= ((sm_double *)sm_engine_eval(sm_expr_get_arg(sme, i), current_cx, sf))->value;
      return (sm_object *)sm_new_double(quotient);
    }
    case sm_pow_expr: {
      sm_double *left_side  = (sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_double *right_side = (sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      return (sm_object *)sm_new_double(pow(left_side->value, right_side->value));
    }
    case sm_sin_expr: {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      return (sm_object *)sm_new_double(sin(left_side->value));
    }
    case sm_cos_expr: {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      return (sm_object *)sm_new_double(cos(left_side->value));
    }
    case sm_tan_expr: {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      return (sm_object *)sm_new_double(tan(left_side->value));
    }
    case sm_sec_expr: {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      return (sm_object *)sm_new_double(1.0 / cos(left_side->value));
    }
    case sm_csc_expr: {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      return (sm_object *)sm_new_double(1.0 / sin(left_side->value));
    }
    case sm_cot_expr: {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      return (sm_object *)sm_new_double(1.0 / tan(left_side->value));
    }
    case sm_sinh_expr: {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      return (sm_object *)sm_new_double(sinh(left_side->value));
    }
    case sm_cosh_expr: {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      return (sm_object *)sm_new_double(cosh(left_side->value));
    }
    case sm_tanh_expr: {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      return (sm_object *)sm_new_double(tanh(left_side->value));
    }
    case sm_sech_expr: {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      return (sm_object *)sm_new_double(1.0 / cosh(left_side->value));
    }
    case sm_csch_expr: {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      return (sm_object *)sm_new_double(1.0 / sinh(left_side->value));
    }
    case sm_coth_expr: {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      return (sm_object *)sm_new_double(1.0 / tanh(left_side->value));
    }
    case sm_ln_expr: {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      return (sm_object *)sm_new_double(log(left_side->value));
    }
    case sm_exp_expr: {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      return (sm_object *)sm_new_double(exp(left_side->value));
    }
    case sm_sqrt_expr: {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      return (sm_object *)sm_new_double(sqrt(left_side->value));
    }
    case sm_abs_expr: {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      double     x         = left_side->value;
      return (sm_object *)sm_new_double(x < 0 ? -1 * x : x);
    }
    case sm_if_expr: {
      sm_object *condition_result = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_symbol *sym              = (sm_symbol *)condition_result;
      if (strncmp(&(sym->name->content), "true", 4) == 0) {
        return sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      }
      return (sm_object *)sm_new_meta((sm_object *)sm_new_symbol(sm_new_string(5, "false")),
                                      current_cx);
    }
    case sm_if_else_expr: {
      sm_object *condition_result = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_symbol *sym              = (sm_symbol *)condition_result;
      if (strncmp(&(sym->name->content), "true", 4) == 0) {
        return sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      }
      return sm_engine_eval(sm_expr_get_arg(sme, 2), current_cx, sf);
    }
    case sm_array_expr: {
      sm_expr *new_arr = sm_new_expr_n(sm_array_expr, sme->size, sme->size);
      for (unsigned int i = 0; i < sme->size; i++) {
        sm_object *new_val = sm_engine_eval(sm_expr_get_arg(sme, i), current_cx, sf);
        sm_expr_set_arg(new_arr, i, new_val);
      }
      return (sm_object *)new_arr;
    }
    case sm_param_list_expr: {
      sm_expr *new_arr = sm_new_expr_n(sm_param_list_expr, sme->size, sme->size);
      for (unsigned int i = 0; i < sme->size; i++) {
        sm_object *new_val = sm_engine_eval(sm_expr_get_arg(sme, i), current_cx, sf);
        sm_expr_set_arg(new_arr, i, new_val);
      }
      return (sm_object *)new_arr;
    }
    case sm_test_lt_expr: {
      sm_object *obj1 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_object *obj2 = sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      if (obj1->my_type == sm_double_type) {
        if (obj2->my_type == sm_double_type) {
          double v1 = ((sm_double *)obj1)->value;
          double v2 = ((sm_double *)obj2)->value;
          if (v1 < v2)
            return (sm_object *)sm_new_symbol(sm_new_string(4, "true"));
          else
            return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
        }
        return (sm_object *)sm_new_meta((sm_object *)sm_new_symbol(sm_new_string(5, "false")),
                                        current_cx);
      }
    }
    case sm_test_gt_expr: {
      sm_object *obj1 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_object *obj2 = sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      if (obj1->my_type == sm_double_type) {
        if (obj2->my_type == sm_double_type) {
          double v1 = ((sm_double *)obj1)->value;
          double v2 = ((sm_double *)obj2)->value;
          if (v1 > v2)
            return (sm_object *)sm_new_symbol(sm_new_string(4, "true"));
          else
            return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
        }
      }
      return (sm_object *)sm_new_meta((sm_object *)sm_new_symbol(sm_new_string(5, "false")),
                                      current_cx);
    }
    case sm_test_eq_expr: {
      sm_object *obj1 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_object *obj2 = sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      if (obj1->my_type == sm_double_type) {
        if (obj2->my_type == sm_double_type) {
          double v1 = ((sm_double *)obj1)->value;
          double v2 = ((sm_double *)obj2)->value;
          if (v1 == v2)
            return (sm_object *)sm_new_symbol(sm_new_string(4, "true"));
          else
            return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
        }
      }
      return (sm_object *)sm_new_meta((sm_object *)sm_new_symbol(sm_new_string(5, "false")),
                                      current_cx);
    }
    default:
      return input;
    } // end of switch inside expr case
  }   // end of expr case
  case sm_meta_type: {
    return ((sm_meta *)input)->address;
  }
  case sm_primitive_type: {
    printf("Primitives are not developed yet\n");
    fflush(stdout);
    return input;
  }
  case sm_symbol_type: {
    sm_symbol                 *sym      = (sm_symbol *)input;
    sm_string                 *var_name = sym->name;
    sm_search_result_cascading sr       = sm_context_find_far(current_cx, var_name);
    if (sr.found == true) {
      return sm_context_entries(sr.context)[sr.index].value;
    } else {
      // should return error object
      printf("Could not find variable: %s\n", &(var_name->content));
      return (sm_object *)sm_new_double(0);
    }
  }
  case sm_local_type: {
    sm_local *local = (sm_local *)input;
    return sm_expr_get_arg(sf, local->index);
  }
  case sm_error_type: {
    // TODO: Add callstack info here.
    return input;
  }
  default:
    return input;
  }
}
