// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "sms.h"

bool is_true(sm_object *obj) {
  if (obj->my_type == SM_SYMBOL_TYPE) {
    sm_symbol *sym = (sm_symbol *)obj;
    if (strncmp(&(sym->name->content), "true", 4) == 0) {
      return true;
    }
  }
  return false;
}

// Recursive engine, uses the C stack for the SMS stack
sm_object *sm_engine_eval(sm_object *input, sm_context *current_cx, sm_expr *sf) {
  switch (input->my_type) {
  case SM_EXPR_TYPE: {
    sm_expr *sme = (sm_expr *)input;
    short    op  = sme->op;
    switch (op) {
    case SM_PARSE_EXPR: {
      sm_object *evaluated = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      if (evaluated->my_type != SM_STRING_TYPE) {
        sm_string *obj_str = sm_object_to_string((sm_object *)evaluated);
        char      *cstr    = &(obj_str->content);
        printf("Attempting to parse something that is not a string: %s\n", cstr);
      }
      sm_string *str     = (sm_string *)evaluated;
      char      *cstr    = &(str->content);
      cstr[str->size]    = ';'; // Temporarily replacing the NULL char
      sm_parse_result pr = sm_parse_cstr(cstr, str->size + 1);
      cstr[str->size]    = '\0'; // Place the null char back
      if (pr.return_val != 0) {
        printf("Error: Parser failed and returned %i\n", pr.return_val);
        return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
      } else
        return pr.parsed_object;
    }
    case SM_TO_STRING_EXPR: {
      sm_object *evaluated = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      return (sm_object *)sm_object_to_string(evaluated);
    }
    case SM_EVAL_EXPR: {
      sm_context *where_to_eval = current_cx;
      if (sme->size > 1) {
        where_to_eval = (sm_context *)sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
        if (where_to_eval->my_type != SM_CONTEXT_TYPE) {
          sm_string *obj_str = sm_object_to_string((sm_object *)where_to_eval);
          char      *cstr    = &(obj_str->content);
          printf("Error: Second argument to eval is not a context: %s\n", cstr);
          return (sm_object *)sm_new_double(0);
        }
      }
      sm_object *evaluated = sm_engine_eval(sm_expr_get_arg(sme, 0), where_to_eval, sf);
      return sm_engine_eval(evaluated, where_to_eval, sf);
    }
    case SM_PRINT_EXPR: {
      sm_object *evaluated = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      if (evaluated->my_type != SM_STRING_TYPE) {
        sm_string *obj_str = sm_object_to_string(evaluated);
        char      *cstr    = &(obj_str->content);
        printf("Error: Trying to print something that is not a string: %s\n", cstr);
      }
      sm_string *smstr = (sm_string *)evaluated;
      printf("%s", &(smstr->content));
      return evaluated;
      break;
    }
    case SM_WHILE_EXPR: {
      sm_expr   *condition  = (sm_expr *)sm_expr_get_arg(sme, 0);
      sm_object *expression = sm_expr_get_arg(sme, 1);
      while (is_true(sm_engine_eval((sm_object *)condition, current_cx, sf))) {
        sm_engine_eval(expression, current_cx, sf);
      }
      return (sm_object *)sm_new_symbol(sm_new_string(4, "true"));
      break;
    }
    case SM_SIZE_EXPR: {
      sm_object *base_obj = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      if (base_obj->my_type != SM_EXPR_TYPE) {
        sm_string *str  = sm_object_to_string(base_obj);
        char      *cstr = &(str->content);
        printf(
          "Error: Size function is applied to an object that is not an array or expression: %s\n",
          cstr);
        return (sm_object *)sm_new_double(-1);
      }
      sm_expr *arr = (sm_expr *)base_obj;
      return (sm_object *)sm_new_double(arr->size);
    }
    case SM_MAP_EXPR: {
      // expecting a unary func
      sm_object *obj0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_object *obj1 = sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      if (obj0->my_type != SM_FUN_TYPE) {
        sm_string *obj_str = sm_object_to_string(obj0);
        char      *cstr    = &(obj_str->content);
        printf("Error: First argument to map should be a function. Instead, %s was provided.\n",
               cstr);
        return (sm_object *)sm_new_double(0);
      }
      if (obj1->my_type != SM_EXPR_TYPE) {
        sm_string *obj_str = sm_object_to_string(obj1);
        char      *cstr    = &(obj_str->content);
        printf("Error: Second argument to map should be an expression or array. Instead, %s was "
               "provided.\n",
               cstr);
        return (sm_object *)sm_new_double(0);
      }
      sm_fun  *fun    = (sm_fun *)obj0;
      sm_expr *arr    = (sm_expr *)obj1;
      sm_expr *output = sm_new_expr_n(arr->op, arr->size, arr->size);
      for (unsigned int i = 0; i < arr->size; i++) {
        sm_object *current_obj = sm_expr_get_arg(arr, i);
        sm_expr   *new_sf      = sm_new_expr(SM_PARAM_LIST_EXPR, current_obj);
        sm_expr_set_arg(output, i, sm_engine_eval(fun->content, fun->parent, new_sf));
      }
      return (sm_object *)output;
    }
    case SM_INDEX_EXPR: {
      sm_object *base_obj = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      if (base_obj->my_type != SM_EXPR_TYPE) {
        sm_string *str  = sm_object_to_string(base_obj);
        char      *cstr = &(str->content);
        printf("Error: Array index expression is applied to an object that is not an array: %s\n",
               cstr);
        return (sm_object *)sm_new_double(0);
      }
      sm_expr   *arr       = (sm_expr *)base_obj;
      sm_object *index_obj = sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      if (index_obj->my_type != SM_DOUBLE_TYPE) {
        sm_string *str  = sm_object_to_string(index_obj);
        char      *cstr = &(str->content);
        printf("Error: Array index is not a number: %s\n", cstr);
        return (sm_object *)sm_new_double(0);
      }
      sm_double   *index_double = (sm_double *)index_obj;
      unsigned int index        = (int)index_double->value;
      if (arr->size < index + 1 || index_double->value < 0) {
        printf("Error: Index out of range: %i\n", index);
        return (sm_object *)sm_new_double(0);
      }
      return sm_expr_get_arg(arr, index);
    }
    case SM_DOT_EXPR: {
      sm_object *base_obj   = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_symbol *field_sym  = (sm_symbol *)sm_expr_get_arg(sme, 1);
      sm_string *field_name = field_sym->name;
      if (base_obj->my_type != SM_CONTEXT_TYPE) {
        sm_string *base_str = sm_object_to_string(base_obj);
        printf("Attempting to apply . to an object that is not a context: %s\n",
               &(base_str->content));
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
    case SM_PARENT_EXPR: {
      sm_object *base_obj = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      if (base_obj->my_type != SM_CONTEXT_TYPE) {
        sm_string *base_str = sm_object_to_string(base_obj);
        printf("Attempting to obtain parent of an object that is not a context: %s\n",
               &(base_str->content));
        return (sm_object *)sm_new_double(0);
      }
      sm_context *cx = (sm_context *)base_obj;
      if (cx->parent == NULL) {
        return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
      }
      return (sm_object *)cx->parent;
    }
    case SM_DIFF_EXPR: {
      sm_object *evaluated0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_object *evaluated1 = sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      if (evaluated1->my_type != SM_SYMBOL_TYPE) {
        printf("Error: Second arg to diff is not a symbol: ");
        sm_string *not_sym_str = sm_object_to_string(evaluated1);
        printf("%s.\n", &(not_sym_str->content));
        return (sm_object *)sm_new_double(0);
      }
      sm_object *result = sm_diff(evaluated0, (sm_symbol *)evaluated1);
      result            = sm_simplify(result);
      return result;
    }
    case SM_SIMP_EXPR: {
      sm_object *evaluated0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      return sm_simplify(evaluated0);
    }
    case SM_FUN_CALL_EXPR: {
      sm_fun         *fun      = (sm_fun *)sm_expr_get_arg(sme, 0);
      struct sm_expr *args     = (struct sm_expr *)sm_expr_get_arg(sme, 1);
      sm_expr        *new_args = (sm_expr *)sm_engine_eval((sm_object *)args, current_cx, sf);
      // When the function did not exist at fun_call creation time
      if (fun->my_type == SM_SYMBOL_TYPE) {
        sm_symbol *fun_sym = (sm_symbol *)fun;
        sm_object *found   = sm_context_get_by_name_far(current_cx, fun_sym->name);
        if (found == NULL) {
          printf("Error: Function not found: %s\n", &(fun_sym->name->content));
          sm_string *err_msg =
            sm_new_string(27 + fun_sym->name->size, "Error: Function not found: ");
          sm_strncpy((&err_msg->content) + 27, &fun_sym->name->content, fun_sym->name->size);
          return (sm_object *)sm_new_error(err_msg, sm_new_string(1, "."), 0);
        }
        if (found->my_type != SM_FUN_TYPE) {
          printf("Error: %s exists, but is not a function.\n", &(fun_sym->name->content));
          sm_string *err_msg = sm_new_string(47 + fun_sym->name->size,
                                             "Error: Variable exists, but is not a function: ");
          sm_strncpy((&err_msg->content) + 47, &fun_sym->name->content, fun_sym->name->size);
          return (sm_object *)sm_new_error(err_msg, sm_new_string(1, "."), 0);
        }
        sm_fun *found_fun = (sm_fun *)found;
        return sm_engine_eval(found_fun->content, found_fun->parent, new_args);
      } else if (fun->my_type == SM_FUN_TYPE) {
        return sm_engine_eval(fun->content, fun->parent, new_args);
      } else {
        return sm_engine_eval((sm_object *)fun, current_cx, new_args);
      }
    }
    case SM_THEN_EXPR: {
      sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      for (unsigned int i = 1; i < sme->size - 1; i++)
        sm_engine_eval(sm_expr_get_arg(sme, i), current_cx, sf);
      return sm_engine_eval(sm_expr_get_arg(sme, sme->size - 1), current_cx, sf);
    }
    case SM_ASSIGN_EXPR: {
      sm_symbol *sym   = (sm_symbol *)sm_expr_get_arg(sme, 0);
      sm_object *value = (sm_object *)sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      sm_context_set(current_cx, sym->name, value);
      return (sm_object *)sm_new_expr_2(SM_ASSIGN_EXPR, (sm_object *)sym, value);
    }
    case SM_PLUS_EXPR: {
      double sum = ((sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf))->value;
      for (unsigned int i = 1; i < sme->size; i++)
        sum += ((sm_double *)sm_engine_eval(sm_expr_get_arg(sme, i), current_cx, sf))->value;
      return (sm_object *)sm_new_double(sum);
    }
    case SM_MINUS_EXPR: {
      sm_object *a = sm_expr_get_arg(sme, 0);
      a            = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      double sum   = ((sm_double *)a)->value;
      for (unsigned int i = 1; i < sme->size; i++)
        sum -= ((sm_double *)sm_engine_eval(sm_expr_get_arg(sme, i), current_cx, sf))->value;
      return (sm_object *)sm_new_double(sum);
    }
    case SM_TIMES_EXPR: {
      double product =
        ((sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf))->value;
      for (unsigned int i = 1; i < sme->size; i++)
        product *= ((sm_double *)sm_engine_eval(sm_expr_get_arg(sme, i), current_cx, sf))->value;
      return (sm_object *)sm_new_double(product);
    }
    case SM_DIVIDE_EXPR: {
      double quotient =
        ((sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf))->value;
      for (unsigned int i = 1; i < sme->size; i++)
        quotient /= ((sm_double *)sm_engine_eval(sm_expr_get_arg(sme, i), current_cx, sf))->value;
      return (sm_object *)sm_new_double(quotient);
    }
    case SM_POW_EXPR: {
      sm_double *left_side  = (sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_double *right_side = (sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      return (sm_object *)sm_new_double(pow(left_side->value, right_side->value));
    }
    case SM_SIN_EXPR: {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      return (sm_object *)sm_new_double(sin(left_side->value));
    }
    case SM_COS_EXPR: {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      return (sm_object *)sm_new_double(cos(left_side->value));
    }
    case SM_TAN_EXPR: {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      return (sm_object *)sm_new_double(tan(left_side->value));
    }
    case SM_SEC_EXPR: {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      return (sm_object *)sm_new_double(1.0 / cos(left_side->value));
    }
    case SM_CSC_EXPR: {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      return (sm_object *)sm_new_double(1.0 / sin(left_side->value));
    }
    case SM_COT_EXPR: {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      return (sm_object *)sm_new_double(1.0 / tan(left_side->value));
    }
    case SM_SINH_EXPR: {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      return (sm_object *)sm_new_double(sinh(left_side->value));
    }
    case SM_COSH_EXPR: {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      return (sm_object *)sm_new_double(cosh(left_side->value));
    }
    case SM_TANH_EXPR: {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      return (sm_object *)sm_new_double(tanh(left_side->value));
    }
    case SM_SECH_EXPR: {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      return (sm_object *)sm_new_double(1.0 / cosh(left_side->value));
    }
    case SM_CSCH_EXPR: {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      return (sm_object *)sm_new_double(1.0 / sinh(left_side->value));
    }
    case SM_COTH_EXPR: {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      return (sm_object *)sm_new_double(1.0 / tanh(left_side->value));
    }
    case SM_LN_EXPR: {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      return (sm_object *)sm_new_double(log(left_side->value));
    }
    case SM_EXP_EXPR: {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      return (sm_object *)sm_new_double(exp(left_side->value));
    }
    case SM_SQRT_EXPR: {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      return (sm_object *)sm_new_double(sqrt(left_side->value));
    }
    case SM_ABS_EXPR: {
      sm_double *left_side = (sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      double     x         = left_side->value;
      return (sm_object *)sm_new_double(x < 0 ? -1 * x : x);
    }
    case SM_IF_EXPR: {
      sm_object *condition_result = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      if (is_true(condition_result)) {
        return sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      }
      return (sm_object *)sm_new_meta((sm_object *)sm_new_symbol(sm_new_string(5, "false")),
                                      current_cx);
    }
    case SM_IF_ELSE_EXPR: {
      sm_object *condition_result = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      if (condition_result->my_type != SM_SYMBOL_TYPE) {
        return sm_engine_eval(sm_expr_get_arg(sme, 2), current_cx, sf);
      }
      if (is_true(condition_result)) {
        return sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      }
      return sm_engine_eval(sm_expr_get_arg(sme, 2), current_cx, sf);
    }
    case SM_ARRAY_EXPR: {
      sm_expr *new_arr = sm_new_expr_n(SM_ARRAY_EXPR, sme->size, sme->size);
      for (unsigned int i = 0; i < sme->size; i++) {
        sm_object *new_val = sm_engine_eval(sm_expr_get_arg(sme, i), current_cx, sf);
        sm_expr_set_arg(new_arr, i, new_val);
      }
      return (sm_object *)new_arr;
    }
    case SM_PARAM_LIST_EXPR: {
      sm_expr *new_arr = sm_new_expr_n(SM_PARAM_LIST_EXPR, sme->size, sme->size);
      for (unsigned int i = 0; i < sme->size; i++) {
        sm_object *new_val = sm_engine_eval(sm_expr_get_arg(sme, i), current_cx, sf);
        sm_expr_set_arg(new_arr, i, new_val);
      }
      return (sm_object *)new_arr;
    }
    case SM_TEST_LT_EXPR: {
      sm_object *obj1 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_object *obj2 = sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      if (obj1->my_type == SM_DOUBLE_TYPE) {
        if (obj2->my_type == SM_DOUBLE_TYPE) {
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
    case SM_TEST_GT_EXPR: {
      sm_object *obj1 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_object *obj2 = sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      if (obj1->my_type == SM_DOUBLE_TYPE) {
        if (obj2->my_type == SM_DOUBLE_TYPE) {
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
    case SM_TEST_EQ_EXPR: {
      sm_object *obj1 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_object *obj2 = sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      if (obj1->my_type == SM_DOUBLE_TYPE) {
        if (obj2->my_type == SM_DOUBLE_TYPE) {
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
  case SM_META_TYPE: {
    return ((sm_meta *)input)->address;
  }
  case SM_PRIMITIVE_TYPE: {
    printf("Primitives are not developed yet\n");
    fflush(stdout);
    return input;
  }
  case SM_SYMBOL_TYPE: {
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
  case SM_LOCAL_TYPE: {
    sm_local *local = (sm_local *)input;
    return sm_expr_get_arg(sf, local->index);
  }
  case SM_ERROR_TYPE: {
    // TODO: Add callstack info here.
    return input;
  }
  default:
    return input;
  }
}
