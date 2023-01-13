// The following file is provided under the BSD 2-clause license. For more info, read LICENSE.txt.
#include "sms.h"

sm_object *sm_engine_eval(sm_object *input) {
  enum sm_object_type input_type = input->my_type;
  switch (input_type) {
  case sm_expr_type: {
    sm_expr *sme = (sm_expr *)input;
    switch (sme->op) {
    case sm_plus: {
      double sum = 0;
      for (unsigned int i = 0; i < sme->size; i++)
        sum += ((sm_double *)sm_engine_eval(sm_get_expr_arg(sme, i)))->value;
      return (sm_object *)sm_new_double(sum);
    }
    case sm_minus: {
      double sum = ((sm_double *)sm_engine_eval(sm_get_expr_arg(sme, 0)))->value;
      for (unsigned int i = 1; i < sme->size; i++)
        sum -= ((sm_double *)sm_engine_eval(sm_get_expr_arg(sme, i)))->value;
      return (sm_object *)sm_new_double(sum);
    }
    case sm_times: {
      double product = ((sm_double *)sm_engine_eval(sm_get_expr_arg(sme, 0)))->value;
      for (unsigned int i = 1; i < sme->size; i++)
        product *= ((sm_double *)sm_engine_eval(sm_get_expr_arg(sme, i)))->value;
      return (sm_object *)sm_new_double(product);
    }
    case sm_divide: {
      double quotient = ((sm_double *)sm_engine_eval(sm_get_expr_arg(sme, 0)))->value;
      for (unsigned int i = 1; i < sme->size; i++)
        quotient /= ((sm_double *)sm_engine_eval(sm_get_expr_arg(sme, i)))->value;
      return (sm_object *)sm_new_double(quotient);
    }
    case sm_pow: {
      sm_double *left_side  = (sm_double *)sm_engine_eval(sm_get_expr_arg(sme, 0));
      sm_double *right_side = (sm_double *)sm_engine_eval(sm_get_expr_arg(sme, 1));
      return (sm_object *)sm_new_double(pow(left_side->value, right_side->value));
    }
    case sm_sin: {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_get_expr_arg(sme, 0));
      return (sm_object *)sm_new_double(sin(left_side->value));
    }
    case sm_cos: {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_get_expr_arg(sme, 0));
      return (sm_object *)sm_new_double(cos(left_side->value));
    }
    case sm_tan: {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_get_expr_arg(sme, 0));
      return (sm_object *)sm_new_double(tan(left_side->value));
    }
    case sm_sec: {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_get_expr_arg(sme, 0));
      return (sm_object *)sm_new_double(1.0 / cos(left_side->value));
      break;
    }
    case sm_csc: {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_get_expr_arg(sme, 0));
      return (sm_object *)sm_new_double(1.0 / sin(left_side->value));
      break;
    }
    case sm_cot: {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_get_expr_arg(sme, 0));
      return (sm_object *)sm_new_double(1.0 / tan(left_side->value));
      break;
    }
    case sm_sinh: {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_get_expr_arg(sme, 0));
      return (sm_object *)sm_new_double(sinh(left_side->value));
    }
    case sm_cosh: {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_get_expr_arg(sme, 0));
      return (sm_object *)sm_new_double(cosh(left_side->value));
    }
    case sm_tanh: {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_get_expr_arg(sme, 0));
      return (sm_object *)sm_new_double(tanh(left_side->value));
    }
    case sm_ln: {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_get_expr_arg(sme, 0));
      return (sm_object *)sm_new_double(log(left_side->value));
    }
    case sm_exp: {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_get_expr_arg(sme, 0));
      return (sm_object *)sm_new_double(exp(left_side->value));
    }
    case sm_sqrt: {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_get_expr_arg(sme, 0));
      return (sm_object *)sm_new_double(sqrt(left_side->value));
    }
    case sm_array: {
      for (unsigned int i = 0; i < sme->size; i++) {
        sm_object *new_val = sm_engine_eval(sm_get_expr_arg(sme, i));
        sm_set_expr_arg(sme, i, new_val);
      }
      return (sm_object *)sme;
    }
    default:
      return input;
    }
  }
  case sm_primitive_type: {
    // sm_expr *smp= (sm_primitive*)input;
    printf("Primitives are not developed yet\n");
    fflush(stdout);
  }
  case sm_symbol_type: {
    sm_symbol    *sym      = (sm_symbol *)input;
    sm_string    *var_name = sym->name;
    search_result sr       = sm_find_var_index(sm_global_context(NULL), var_name);
    if (sr.found == true) {
      return sm_context_entries(sm_global_context(NULL))[sr.index].value;
    } else {
      // should return error object;
      printf("Could not find variable: %s\n", &(var_name->content));
      return (sm_object *)sm_new_double(0);
    }
  }
  case sm_context_type: {
    sm_context       *cx = (sm_context *)input;
    sm_context_entry *ce = sm_context_entries(cx);
    for (unsigned int i = 0; i < cx->size; i++) {
      if (((sm_context *)ce[i].value) != cx)
        ce[i].value = sm_engine_eval(ce[i].value);
    }
  }
  default: // literals are passed through
    return input;
  }
}
