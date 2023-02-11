// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "sms.h"

// TODO:store the engine command set in an array of c function ptrs
//  execute the engine this way: evaluated = (sm_global_engine_cmd_set(NULL)[op])(value);
// so that we are not wasting time with if conditions

// Recursive engine, uses the C stack for the SMS stack
sm_object *sm_engine_eval(sm_object *input, sm_context *current_cx) {
  short int input_type = input->my_type;
  if (sm_object_is_literal(input->my_type) == true) {
    return input;
  }
  if (input->my_type == sm_expr_type) {
    sm_expr *sme = (sm_expr *)input;
    short    op  = sme->op;

    switch (op) {
    case (sm_then): {
      sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx);
      for (unsigned int i = 1; i < sme->size - 1; i++)
        sm_engine_eval(sm_expr_get_arg(sme, i), current_cx);
      return sm_engine_eval(sm_expr_get_arg(sme, sme->size - 1), current_cx);
    }
    case (sm_assign): {
      sm_symbol  *sym    = (sm_symbol *)sm_expr_get_arg(sme, 0);
      sm_object  *value  = (sm_object *)sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx);
      sm_context *new_cx = sm_context_set(current_cx, sym->name, value);
      sm_context_update_relatives(new_cx, current_cx);
      return value;
    }
    case (sm_plus): {
      double sum = ((sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx))->value;
      for (unsigned int i = 1; i < sme->size; i++)
        sum += ((sm_double *)sm_engine_eval(sm_expr_get_arg(sme, i), current_cx))->value;
      return (sm_object *)sm_new_double(sum);
    }
    case (sm_minus): {
      sm_object *a = sm_expr_get_arg(sme, 0);
      a            = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx);
      double sum   = ((sm_double *)a)->value;
      for (unsigned int i = 1; i < sme->size; i++)
        sum -= ((sm_double *)sm_engine_eval(sm_expr_get_arg(sme, i), current_cx))->value;
      return (sm_object *)sm_new_double(sum);
    }
    case (sm_times): {
      double product = ((sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx))->value;
      for (unsigned int i = 1; i < sme->size; i++)
        product *= ((sm_double *)sm_engine_eval(sm_expr_get_arg(sme, i), current_cx))->value;
      return (sm_object *)sm_new_double(product);
    }
    case (sm_divide): {
      double quotient = ((sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx))->value;
      for (unsigned int i = 1; i < sme->size; i++)
        quotient /= ((sm_double *)sm_engine_eval(sm_expr_get_arg(sme, i), current_cx))->value;
      return (sm_object *)sm_new_double(quotient);
    }
    case (sm_pow): {
      sm_double *left_side  = (sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx);
      sm_double *right_side = (sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx);
      return (sm_object *)sm_new_double(pow(left_side->value, right_side->value));
    }
    case (sm_sin): {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx);
      return (sm_object *)sm_new_double(sin(left_side->value));
    }
    case (sm_cos): {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx);
      return (sm_object *)sm_new_double(cos(left_side->value));
    }
    case (sm_tan): {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx);
      return (sm_object *)sm_new_double(tan(left_side->value));
    }
    case (sm_sec): {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx);
      return (sm_object *)sm_new_double(1.0 / cos(left_side->value));
    }
    case (sm_csc): {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx);
      return (sm_object *)sm_new_double(1.0 / sin(left_side->value));
    }
    case (sm_cot): {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx);
      return (sm_object *)sm_new_double(1.0 / tan(left_side->value));
    }
    case (sm_sinh): {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx);
      return (sm_object *)sm_new_double(sinh(left_side->value));
    }
    case (sm_cosh): {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx);
      return (sm_object *)sm_new_double(cosh(left_side->value));
    }
    case (sm_tanh): {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx);
      return (sm_object *)sm_new_double(tanh(left_side->value));
    }
    case (sm_ln): {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx);
      return (sm_object *)sm_new_double(log(left_side->value));
    }
    case (sm_exp): {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx);
      return (sm_object *)sm_new_double(exp(left_side->value));
    }
    case (sm_sqrt): {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx);
      return (sm_object *)sm_new_double(sqrt(left_side->value));
    }
    case (sm_abs): {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx);
      double     x         = left_side->value;
      return (sm_object *)sm_new_double(x < 0 ? -1 * x : x);
    }
    case (sm_if): {
      sm_object *condition_result = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx);
      if (condition_result->my_type == sm_meta_type) {
        sm_meta *meta = (sm_meta *)condition_result;
        if (meta->address->my_type == sm_symbol_type) {
          sm_symbol *sym = (sm_symbol *)meta->address;
          if (strncmp(&(sym->name->content), "true", 4) == 0) {
            return sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx);
          }
        }
      }
      return (sm_object *)sm_new_meta((sm_object *)sm_new_symbol(sm_new_string(5, "false")),
                                      current_cx);
    }
    case (sm_if_else): {
      sm_object *condition_result = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx);
      if (condition_result->my_type == sm_meta_type) {
        sm_meta *meta = (sm_meta *)condition_result;
        if (meta->address->my_type == sm_symbol_type) {
          sm_symbol *sym = (sm_symbol *)meta->address;
          if (strncmp(&(sym->name->content), "true", 4) == 0) {
            return sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx);
          }
        }
      }
      return sm_engine_eval(sm_expr_get_arg(sme, 2), current_cx);
    }
    case (sm_array): {
      sm_expr *new_arr = sm_new_expr_n(sm_array, sme->size, sme->capacity);
      for (unsigned int i = 0; i < sme->size; i++) {
        sm_object *new_val = sm_engine_eval(sm_expr_get_arg(sme, i), current_cx);
        sm_set_expr_arg(new_arr, i, new_val);
      }
      return (sm_object *)new_arr;
    }
    case (sm_funcall_l_l): {
      sm_meta    *meta  = (sm_meta *)sm_expr_get_arg(sme, 0);
      sm_context *cx    = (sm_context *)sm_expr_get_arg(sme, 1);
      cx                = (sm_context *)sm_engine_eval((sm_object *)cx, cx);
      sm_object *result = sm_engine_eval(meta->address, cx);
      return result;
    }
    case (sm_funcall_v_l): {
      sm_symbol  *sym = (sm_symbol *)sm_expr_get_arg(sme, 0);
      sm_context *cx  = (sm_context *)sm_expr_get_arg(sme, 1);

      sm_context *new_cx = (sm_context *)sm_engine_eval((sm_object *)cx, current_cx);
      sm_context_update_relatives(new_cx, cx);
      cx = new_cx;

      sm_string                 *var_name = sym->name;
      sm_search_result_cascading sr       = sm_context_find_far(cx, var_name);

      if (sr.found == true) {
        sm_meta   *meta   = (sm_meta *)sm_context_entries(sr.context)[sr.index].value;
        sm_object *result = sm_engine_eval(meta->address, cx);
        return result;
      } else {
        // should return error object;
        printf("Could not find function: %s\n", &(var_name->content));
        return (sm_object *)sm_new_double(0);
      }
    }
    case (sm_funcall_l_v): {
      sm_meta                   *meta     = (sm_meta *)sm_expr_get_arg(sme, 0);
      sm_symbol                 *sym      = (sm_symbol *)sm_expr_get_arg(sme, 1);
      sm_string                 *var_name = sym->name;
      sm_search_result_cascading sr       = sm_context_find_far(current_cx, var_name);
      if (sr.found == true) {
        sm_context *cx     = (sm_context *)sm_context_entries(sr.context)[sr.index].value;
        sm_object  *result = sm_engine_eval(meta->address, cx);
        return result;
      } else {
        // should return error object;
        printf("Could not find context: %s\n", &(var_name->content));

        return (sm_object *)sm_new_double(0);
      }
    }
    case (sm_funcall_v_v): {
      sm_object                 *obj1      = sm_expr_get_arg(sme, 0);
      sm_object                 *obj2      = sm_expr_get_arg(sme, 1);
      sm_string                 *var1_name = ((sm_symbol *)obj1)->name;
      sm_string                 *var2_name = ((sm_symbol *)obj2)->name;
      sm_search_result_cascading sr1       = sm_context_find_far(current_cx, var1_name);
      sm_search_result_cascading sr2       = sm_context_find_far(current_cx, var2_name);
      if (sr1.found == true && sr2.found == true) {
        sm_meta    *meta   = (sm_meta *)sm_context_entries(sr1.context)[sr1.index].value;
        sm_context *cx     = (sm_context *)sm_context_entries(sr1.context)[sr2.index].value;
        sm_object  *result = sm_engine_eval(meta->address, cx);
        return result;
      } else {
        // should return error object;
        if (sr1.found != true)
          printf("Could not find function: %s\n", &(var1_name->content));
        if (sr2.found != true)
          printf("Could not find context: %s\n", &(var2_name->content));
        return (sm_object *)sm_new_double(0);
      }
    }
    case (sm_test_lt): {
      sm_object *obj1 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx);
      sm_object *obj2 = sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx);
      if (obj1->my_type == sm_double_type) {
        if (obj2->my_type == sm_double_type) {
          double v1 = ((sm_double *)obj1)->value;
          double v2 = ((sm_double *)obj2)->value;
          if (v1 < v2)
            return (sm_object *)sm_new_meta((sm_object *)sm_new_symbol(sm_new_string(4, "true")),
                                            current_cx);
          else
            return (sm_object *)sm_new_meta((sm_object *)sm_new_symbol(sm_new_string(5, "false")),
                                            current_cx);
        }
        return (sm_object *)sm_new_meta((sm_object *)sm_new_symbol(sm_new_string(5, "false")),
                                        current_cx);
      }
    }
    case (sm_test_gt): {
      sm_object *obj1 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx);
      sm_object *obj2 = sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx);
      if (obj1->my_type == sm_double_type) {
        if (obj2->my_type == sm_double_type) {
          double v1 = ((sm_double *)obj1)->value;
          double v2 = ((sm_double *)obj2)->value;

          if (v1 > v2)
            return (sm_object *)sm_new_meta((sm_object *)sm_new_symbol(sm_new_string(4, "true")),
                                            current_cx);
          else
            return (sm_object *)sm_new_meta((sm_object *)sm_new_symbol(sm_new_string(5, "false")),
                                            current_cx);
        }
      }
      return (sm_object *)sm_new_meta((sm_object *)sm_new_symbol(sm_new_string(5, "false")),
                                      current_cx);
    }
    case (sm_test_eq): {
      sm_object *obj1 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx);
      sm_object *obj2 = sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx);
      if (obj1->my_type == sm_double_type) {
        if (obj2->my_type == sm_double_type) {
          double v1 = ((sm_double *)obj1)->value;
          double v2 = ((sm_double *)obj2)->value;
          if (v1 == v2)
            return (sm_object *)sm_new_meta((sm_object *)sm_new_symbol(sm_new_string(4, "true")),
                                            current_cx);
          else
            return (sm_object *)sm_new_meta((sm_object *)sm_new_symbol(sm_new_string(5, "false")),
                                            current_cx);
        }
      }
      return (sm_object *)sm_new_meta((sm_object *)sm_new_symbol(sm_new_string(5, "false")),
                                      current_cx);
    }
    default:
      // default action for expr
      return input;
    } // end of expr case
  }
  // non-expression
  switch (input_type) {
  case (sm_primitive_type): {
    printf("Primitives are not developed yet\n");
    fflush(stdout);
    return input;
  }
  case (sm_symbol_type): {
    sm_symbol                 *sym      = (sm_symbol *)input;
    sm_string                 *var_name = sym->name;
    sm_search_result_cascading sr       = sm_context_find_far(current_cx, var_name);
    if (sr.found == true) {
      return sm_context_entries(sr.context)[sr.index].value;
    } else {
      // should return error object;
      printf("Could not find variable: %s\n", &(var_name->content));
      return (sm_object *)sm_new_double(0);
    }
  }
  case (sm_context_type): {
    sm_context       *cx     = (sm_context *)input;
    sm_context_entry *ce     = sm_context_entries(cx);
    sm_context       *new_cx = sm_new_context(cx->size, cx->size, cx->parent);
    new_cx->children         = cx->children;
    sm_context_entry *new_ce = sm_context_entries(new_cx);
    for (unsigned int i = 0; i < cx->size; i++) {
      new_ce[i].name = ce[i].name;
      if (false == sm_object_is_literal(ce[i].value->my_type) &&
          ((sm_context *)ce[i].value) != cx) {
        new_ce[i].value = sm_engine_eval(ce[i].value, current_cx);
      } else {
        new_ce[i].value = ce[i].value;
      }
    }
    sm_context_update_relatives(new_cx, cx);
    return (sm_object *)new_cx;
  }
  default:
    return input;
  }
  return input;
}
