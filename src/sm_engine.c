#include "sms.h"

sm_object *sm_engine_eval(sm_object *input) {
  enum sm_object_type input_type = input->my_type;
  switch (input_type) {
  case sm_expression_type: {
    sm_expression *sme = (sm_expression *)input;
    switch (sme->op) {
    case sm_plus: {
      sm_double *left_side  = (sm_double *)sm_engine_eval(sm_get_expression_arg(sme, 0));
      sm_double *right_side = (sm_double *)sm_engine_eval(sm_get_expression_arg(sme, 1));
      return (sm_object *)sm_new_double(left_side->value + right_side->value);
    }
    case sm_minus: {
      sm_double *left_side  = (sm_double *)sm_engine_eval(sm_get_expression_arg(sme, 0));
      sm_double *right_side = (sm_double *)sm_engine_eval(sm_get_expression_arg(sme, 1));
      return (sm_object *)sm_new_double(left_side->value - right_side->value);
    }
    case sm_times: {
      sm_double *left_side  = (sm_double *)sm_engine_eval(sm_get_expression_arg(sme, 0));
      sm_double *right_side = (sm_double *)sm_engine_eval(sm_get_expression_arg(sme, 1));
      return (sm_object *)sm_new_double(left_side->value * right_side->value);
    }
    case sm_divide: {
      sm_double *left_side  = (sm_double *)sm_engine_eval(sm_get_expression_arg(sme, 0));
      sm_double *right_side = (sm_double *)sm_engine_eval(sm_get_expression_arg(sme, 1));
      return (sm_object *)sm_new_double(left_side->value / right_side->value);
    }
    case sm_pow: {
      sm_double *left_side  = (sm_double *)sm_engine_eval(sm_get_expression_arg(sme, 0));
      sm_double *right_side = (sm_double *)sm_engine_eval(sm_get_expression_arg(sme, 1));
      return (sm_object *)sm_new_double(pow(left_side->value, right_side->value));
    }
    case sm_sin: {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_get_expression_arg(sme, 0));
      return (sm_object *)sm_new_double(sin(left_side->value));
    }
    case sm_cos: {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_get_expression_arg(sme, 0));
      return (sm_object *)sm_new_double(cos(left_side->value));
    }
    case sm_tan: {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_get_expression_arg(sme, 0));
      return (sm_object *)sm_new_double(tan(left_side->value));
    }
    case sm_sec: {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_get_expression_arg(sme, 0));
      // we may need to manually invert these 3 trig functions.
      // return (sm_object *)sm_new_double(sec(left_side->value));
    }
    case sm_csc: {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_get_expression_arg(sme, 0));
      // return (sm_object *)sm_new_double(csc(left_side->value));
    }
    case sm_cot: {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_get_expression_arg(sme, 0));
      // return (sm_object *)sm_new_double(cot(left_side->value));
    }
    case sm_sinh: {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_get_expression_arg(sme, 0));
      return (sm_object *)sm_new_double(sinh(left_side->value));
    }
    case sm_cosh: {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_get_expression_arg(sme, 0));
      return (sm_object *)sm_new_double(cosh(left_side->value));
    }
    case sm_tanh: {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_get_expression_arg(sme, 0));
      return (sm_object *)sm_new_double(tanh(left_side->value));
    }
    case sm_ln: {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_get_expression_arg(sme, 0));
      return (sm_object *)sm_new_double(log(left_side->value));
    }
    case sm_exp: {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_get_expression_arg(sme, 0));
      return (sm_object *)sm_new_double(exp(left_side->value));
    }
    case sm_sqrt: {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_get_expression_arg(sme, 0));
      return (sm_object *)sm_new_double(sqrt(left_side->value));
    }

    default:
      return input;
    }
  }
  case sm_primitive_type: {
    // sm_expression *smp= (sm_primitive*)input;
    printf("Primitives are not developed yet\n");
    fflush(stdout);
  }
  case sm_symbol_type: {
    sm_symbol    *sym      = (sm_symbol *)input;
    sm_string    *var_name = sym->name;
    search_result sr       = sm_find_var_index(sm_global_context(NULL), var_name);
    if (sr.found == true) {
      return get_context_entries(sm_global_context(NULL))[sr.index].value;
    } else {
      // should return error object;
      printf("Could not find variable: %s\n", &(var_name->content));
      return (sm_object *)sm_new_double(0);
    }
  }
  default: // literals are passed through
    return input;
  }
}
