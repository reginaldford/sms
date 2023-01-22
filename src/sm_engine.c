// The following file is provided under the BSD 2-clause license. For more info, read LICENSE.txt.

#include "sms.h"

sm_object *sm_engine_eval(sm_object *input) {
  short int input_type = input->my_type;
  if (input_type == sm_expr_type) {
    sm_expr *sme = (sm_expr *)input;
    short    op  = sme->op;

    if (op == sm_assign) {
      sm_symbol  *sym   = (sm_symbol *)sm_get_expr_arg(sme, 0);
      sm_object  *value = (sm_object *)sm_get_expr_arg(sme, 1);
      sm_context *cx    = sm_set_var(sm_global_context(NULL), sym->name, sm_engine_eval(value));
      sm_global_context(cx);
      return value;
    }
    if (op == sm_plus) {
      double sum = ((sm_double *)sm_engine_eval(sm_get_expr_arg(sme, 0)))->value;
      for (unsigned int i = 1; i < sme->size; i++)
        sum += ((sm_double *)sm_engine_eval(sm_get_expr_arg(sme, i)))->value;
      return (sm_object *)sm_new_double(sum);
    }
    if (op == sm_minus) {
      double sum = ((sm_double *)sm_engine_eval(sm_get_expr_arg(sme, 0)))->value;
      for (unsigned int i = 1; i < sme->size; i++)
        sum -= ((sm_double *)sm_engine_eval(sm_get_expr_arg(sme, i)))->value;
      return (sm_object *)sm_new_double(sum);
    }
    if (op == sm_times) {
      double product = ((sm_double *)sm_engine_eval(sm_get_expr_arg(sme, 0)))->value;
      for (unsigned int i = 1; i < sme->size; i++)
        product *= ((sm_double *)sm_engine_eval(sm_get_expr_arg(sme, i)))->value;
      return (sm_object *)sm_new_double(product);
    }
    if (op == sm_divide) {
      double quotient = ((sm_double *)sm_engine_eval(sm_get_expr_arg(sme, 0)))->value;
      for (unsigned int i = 1; i < sme->size; i++)
        quotient /= ((sm_double *)sm_engine_eval(sm_get_expr_arg(sme, i)))->value;
      return (sm_object *)sm_new_double(quotient);
    }
    if (op == sm_pow) {
      sm_double *left_side  = (sm_double *)sm_engine_eval(sm_get_expr_arg(sme, 0));
      sm_double *right_side = (sm_double *)sm_engine_eval(sm_get_expr_arg(sme, 1));
      return (sm_object *)sm_new_double(pow(left_side->value, right_side->value));
    }
    if (op == sm_sin) {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_get_expr_arg(sme, 0));
      return (sm_object *)sm_new_double(sin(left_side->value));
    }
    if (op == sm_cos) {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_get_expr_arg(sme, 0));
      return (sm_object *)sm_new_double(cos(left_side->value));
    }
    if (op == sm_tan) {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_get_expr_arg(sme, 0));
      return (sm_object *)sm_new_double(tan(left_side->value));
    }
    if (op == sm_sec) {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_get_expr_arg(sme, 0));
      return (sm_object *)sm_new_double(1.0 / cos(left_side->value));
    }
    if (op == sm_csc) {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_get_expr_arg(sme, 0));
      return (sm_object *)sm_new_double(1.0 / sin(left_side->value));
    }
    if (op == sm_cot) {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_get_expr_arg(sme, 0));
      return (sm_object *)sm_new_double(1.0 / tan(left_side->value));
    }
    if (op == sm_sinh) {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_get_expr_arg(sme, 0));
      return (sm_object *)sm_new_double(sinh(left_side->value));
    }
    if (op == sm_cosh) {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_get_expr_arg(sme, 0));
      return (sm_object *)sm_new_double(cosh(left_side->value));
    }
    if (op == sm_tanh) {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_get_expr_arg(sme, 0));
      return (sm_object *)sm_new_double(tanh(left_side->value));
    }
    if (op == sm_ln) {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_get_expr_arg(sme, 0));
      return (sm_object *)sm_new_double(log(left_side->value));
    }
    if (op == sm_exp) {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_get_expr_arg(sme, 0));
      return (sm_object *)sm_new_double(exp(left_side->value));
    }
    if (op == sm_sqrt) {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_get_expr_arg(sme, 0));
      return (sm_object *)sm_new_double(sqrt(left_side->value));
    }
    if (op == sm_if) {
      sm_object *condition_result = sm_engine_eval(sm_get_expr_arg(sme, 0));
      if (condition_result->my_type == sm_meta_type) {
        sm_meta *meta = (sm_meta *)condition_result;
        if (meta->address->my_type == sm_symbol_type) {
          sm_symbol *sym = (sm_symbol *)meta->address;
          if (strncmp(&(sym->name->content), "true", 4) == 0) {
            return sm_engine_eval(sm_get_expr_arg(sme, 1));
          }
        }
      }
      // what to return??
      return (sm_object *)sm_new_meta((sm_object *)sm_new_symbol(sm_new_string(5, "false")));
    }
    if (op == sm_if_else) {
      sm_object *condition_result = sm_engine_eval(sm_get_expr_arg(sme, 0));
      if (condition_result->my_type == sm_meta_type) {
        sm_meta *meta = (sm_meta *)condition_result;
        if (meta->address->my_type == sm_symbol_type) {
          sm_symbol *sym = (sm_symbol *)meta->address;
          if (strncmp(&(sym->name->content), "true", 4) == 0) {
            return sm_engine_eval(sm_get_expr_arg(sme, 1));
          }
        }
      }
      return sm_engine_eval(sm_get_expr_arg(sme, 2));
    }
    if (op == sm_array) {
      for (unsigned int i = 0; i < sme->size; i++) {
        sm_object *new_val = sm_engine_eval(sm_get_expr_arg(sme, i));
        sm_set_expr_arg(sme, i, new_val);
      }
      return (sm_object *)sme;
    }
    if (op == sm_funcall_l_l) {
      sm_meta    *meta        = (sm_meta *)sm_get_expr_arg(sme, 0);
      sm_context *cx          = (sm_context *)sm_get_expr_arg(sme, 1);
      sm_context *previous_cx = sm_global_context(cx);
      sm_object  *result      = sm_engine_eval(meta->address);
      sm_global_context(previous_cx);
      return result;
    }
    if (op == sm_funcall_v_l) {
      sm_symbol                 *sym      = (sm_symbol *)sm_get_expr_arg(sme, 0);
      sm_context                *cx       = (sm_context *)sm_get_expr_arg(sme, 1);
      sm_string                 *var_name = sym->name;
      sm_search_result_cascading sr = sm_find_var_cascading(sm_global_context(NULL), var_name);
      if (sr.found == true) {
        sm_meta    *meta        = (sm_meta *)sm_context_entries(sr.context)[sr.index].value;
        sm_context *previous_cx = sm_global_context(cx);
        sm_object  *result      = sm_engine_eval(meta->address);
        sm_global_context(previous_cx);
        return result;
      } else {
        // should return error object;
        printf("Could not find function: %s\n", &(var_name->content));
        return (sm_object *)sm_new_double(0);
      }
    }
    if (op == sm_funcall_l_v) {
      sm_meta                   *meta     = (sm_meta *)sm_get_expr_arg(sme, 0);
      sm_symbol                 *sym      = (sm_symbol *)sm_get_expr_arg(sme, 1);
      sm_string                 *var_name = sym->name;
      sm_search_result_cascading sr = sm_find_var_cascading(sm_global_context(NULL), var_name);
      if (sr.found == true) {
        sm_context *cx       = (sm_context *)sm_context_entries(sr.context)[sr.index].value;
        sm_context *previous = sm_global_context(cx);
        sm_object  *result   = sm_engine_eval(meta->address);
        sm_global_context(previous);
        return result;
      } else {
        // should return error object;
        printf("Could not find context: %s\n", &(var_name->content));
        return (sm_object *)sm_new_double(0);
      }
    }
    if (op == sm_funcall_v_v) {
      sm_object                 *obj1      = sm_get_expr_arg(sme, 0);
      sm_object                 *obj2      = sm_get_expr_arg(sme, 1);
      sm_string                 *var1_name = ((sm_symbol *)obj1)->name;
      sm_string                 *var2_name = ((sm_symbol *)obj2)->name;
      sm_search_result_cascading sr1 = sm_find_var_cascading(sm_global_context(NULL), var1_name);
      sm_search_result_cascading sr2 = sm_find_var_cascading(sm_global_context(NULL), var2_name);
      if (sr1.found == true && sr2.found == true) {
        sm_meta    *meta     = (sm_meta *)sm_context_entries(sr1.context)[sr1.index].value;
        sm_context *cx       = (sm_context *)sm_context_entries(sr1.context)[sr2.index].value;
        sm_context *previous = sm_global_context(cx);
        sm_object  *result   = sm_engine_eval(meta->address);
        sm_global_context(previous);
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
    if (op == sm_test_lt) {
      sm_object *obj1 = sm_get_expr_arg(sme, 0);
      sm_object *obj2 = sm_get_expr_arg(sme, 1);
      if (obj1->my_type == sm_double_type) {
        if (obj2->my_type == sm_double_type) {
          double v1 = ((sm_double *)obj1)->value;
          double v2 = ((sm_double *)obj2)->value;
          if (v1 < v2)
            return (sm_object *)sm_new_meta((sm_object *)sm_new_symbol(sm_new_string(4, "true")));
          else
            return (sm_object *)sm_new_meta((sm_object *)sm_new_symbol(sm_new_string(5, "false")));
        }
        return (sm_object *)sm_new_meta((sm_object *)sm_new_symbol(sm_new_string(5, "false")));
      }
    }
    if (op == sm_test_gt) {
      sm_object *obj1 = sm_get_expr_arg(sme, 0);
      sm_object *obj2 = sm_get_expr_arg(sme, 1);
      if (obj1->my_type == sm_double_type) {
        if (obj2->my_type == sm_double_type) {
          double v1 = ((sm_double *)obj1)->value;
          double v2 = ((sm_double *)obj2)->value;
          if (v1 > v2)
            return (sm_object *)sm_new_meta((sm_object *)sm_new_symbol(sm_new_string(4, "true")));
          else
            return (sm_object *)sm_new_meta((sm_object *)sm_new_symbol(sm_new_string(5, "false")));
        }
      }
      return (sm_object *)sm_new_meta((sm_object *)sm_new_symbol(sm_new_string(5, "false")));
    }
    if (op == sm_test_eq) {
      sm_object *obj1 = sm_get_expr_arg(sme, 0);
      sm_object *obj2 = sm_get_expr_arg(sme, 1);
      if (obj1->my_type == sm_double_type) {
        if (obj2->my_type == sm_double_type) {
          double v1 = ((sm_double *)obj1)->value;
          double v2 = ((sm_double *)obj2)->value;
          if (v1 == v2)
            return (sm_object *)sm_new_meta((sm_object *)sm_new_symbol(sm_new_string(4, "true")));
          else
            return (sm_object *)sm_new_meta((sm_object *)sm_new_symbol(sm_new_string(5, "false")));
        }
      }
      return (sm_object *)sm_new_meta((sm_object *)sm_new_symbol(sm_new_string(5, "false")));
    }
    // default action for expr
    return input;
  } // end of expr case

  // other objects
  if (input_type == sm_primitive_type) {
    // sm_expr *smp= (sm_primitive*)input;
    printf("Primitives are not developed yet\n");
    fflush(stdout);
    return input;
  }
  if (input_type == sm_symbol_type) {
    sm_symbol                 *sym      = (sm_symbol *)input;
    sm_string                 *var_name = sym->name;
    sm_search_result_cascading sr       = sm_find_var_cascading(sm_global_context(NULL), var_name);
    if (sr.found == true) {
      return sm_context_entries(sr.context)[sr.index].value;
    } else {
      // should return error object;
      printf("Could not find variable: %s\n", &(var_name->content));
      return (sm_object *)sm_new_double(0);
    }
  }
  if (input_type == sm_context_type) {
    sm_context       *cx = (sm_context *)input;
    sm_context_entry *ce = sm_context_entries(cx);
    for (unsigned int i = 0; i < cx->size; i++) {
      if (((sm_context *)ce[i].value) != cx)
        ce[i].value = sm_engine_eval(ce[i].value);
    }
    return input;
  }
  return input;
}
