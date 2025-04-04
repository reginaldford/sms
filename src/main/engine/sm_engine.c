// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"

// Globals from sm_global.c
extern sm_heap_set *sms_all_heaps;
extern sm_heap     *sms_heap;
extern sm_heap     *sms_other_heap;
extern sm_heap     *sms_symbol_heap;
extern sm_heap     *sms_symbol_name_heap;
extern sm_symbol   *sms_true;
extern sm_symbol   *sms_false;
extern sm_object   *return_obj;
extern sm_stack2   *sms_stack;
extern sm_stack2   *sms_cx_stack;

// Basic type checking

// Evaluate the argument, then run type check
static inline sm_object *eager_type_check(sm_expr *sme, uint32_t operand, uint32_t param_type) {
  sm_object *obj = sm_eval(sm_expr_get_arg(sme, operand));
  if (param_type != obj->my_type) {
    sm_string *source  = (sm_string *)sm_cx_get(sme->notes, sm_new_symbol("source", 6));
    sm_f64    *line    = (sm_f64 *)sm_cx_get(sme->notes, sm_new_symbol("line", 4));
    sm_string *message = sm_new_fstring_at(
      sms_heap, "Wrong type for argument %i on %s. Argument type is: %s , but Expected: %s (%s:%i)",
      operand, sm_global_fn_name(sme->op), sm_type_name(obj->my_type), sm_type_name(param_type),
      __FILE__, __LINE__);
    sm_error *err = sm_new_error(12, "typeMismatch", message->size, &message->content, source->size,
                                 &source->content, (uint32_t)line->value);
    if (param_type == SM_ERR_TYPE) {
      err->origin = err;
    }
    return ((sm_object *)err);
  }
  return (sm_object *)obj;
}

// Evaluate the argument, then run type check. 2 possibilities allowed
static inline sm_object *eager_type_check2(sm_expr *sme, uint32_t operand, uint32_t param_type1,
                                           uint32_t param_type2) {
  return_obj = sm_expr_get_arg(sme, operand);
  sm_eval(return_obj);
  sm_object *obj = return_obj;
  if (param_type1 != obj->my_type && param_type2 != obj->my_type) {
    sm_string *source  = (sm_string *)sm_cx_get(sme->notes, sm_new_symbol("source", 6));
    sm_f64    *line    = (sm_f64 *)sm_cx_get(sme->notes, sm_new_symbol("line", 4));
    sm_string *message = sm_new_fstring_at(
      sms_heap, "Wrong type for argument %i on %s. Argument type is: %s , but Expected: %s or %s",
      operand, sm_global_fn_name(sme->op), sm_type_name(obj->my_type), sm_type_name(param_type1),
      sm_type_name(param_type2));
    sm_error *err = sm_new_error(12, "typeMismatch", message->size, &message->content, source->size,
                                 &source->content, (uint32_t)line->value);
    if (param_type1 == SM_ERR_TYPE || param_type2 == SM_ERR_TYPE) {
      err->origin = err;
    }
    return ((sm_object *)err);
  }
  return (obj);
}

// Evaluate the argument, then run type check. 3 possibilities allowed
static inline sm_object *eager_type_check3(sm_expr *sme, uint32_t operand, uint32_t param_type1,
                                           uint32_t param_type2, uint32_t param_type3) {
  return_obj = sm_expr_get_arg(sme, operand);
  sm_eval(return_obj);
  sm_object *obj = return_obj;
  if (param_type1 != obj->my_type && param_type2 != obj->my_type && param_type3 != obj->my_type) {
    sm_string *source  = (sm_string *)sm_cx_get(sme->notes, sm_new_symbol("source", 6));
    sm_f64    *line    = (sm_f64 *)sm_cx_get(sme->notes, sm_new_symbol("line", 4));
    sm_string *message = sm_new_fstring_at(
      sms_heap,
      "Wrong type for argument %i on %s. Argument type is: %s "
      ", but Expected: %s, %s, or %s (%s:%i)",
      operand, sm_global_fn_name(sme->op), sm_type_name(obj->my_type), sm_type_name(param_type1),
      sm_type_name(param_type2), sm_type_name(param_type3), __FILE__, __LINE__);
    sm_error *err = sm_new_error(12, "typeMismatch", message->size, &message->content, source->size,
                                 &source->content, (uint32_t)line->value);
    if (param_type1 == SM_ERR_TYPE || param_type2 == SM_ERR_TYPE || param_type3 == SM_ERR_TYPE) {
      err->origin = err;
    }
    return ((sm_object *)err);
  }
  return (obj);
}


sm_object *sm_eval(sm_object *input) {
  switch (input->my_type) {
  case SM_EXPR_TYPE: {
    sm_expr *sme = (sm_expr *)input;
    uint32_t op  = sme->op;
    switch (op) {
    case SM_VERSION_EXPR: {
      return (((sm_object *)sms_global_version()));
      break;
    }
    case SM_NEW_F64_EXPR: {
      // TODO stack push to sms_stack every such var:
      sm_object *fromObj = eager_type_check3(sme, 0, SM_F64_TYPE, SM_UI8_TYPE, SM_STRING_TYPE);
      if (fromObj->my_type == SM_ERR_TYPE)
        return fromObj;
      switch (fromObj->my_type) {
      case SM_F64_TYPE:
        return (((sm_object *)sm_new_f64(((sm_f64 *)fromObj)->value)));
      case SM_UI8_TYPE:
        return (((sm_object *)sm_new_f64(((sm_ui8 *)fromObj)->value)));
      case SM_STRING_TYPE: {
        char       *endptr;
        const char *str_content = &((sm_string *)fromObj)->content;
        double      value       = strtod(str_content, &endptr);
        // Check for conversion errors
        if (endptr == str_content) {
          sm_symbol *title = sm_new_symbol("cannotConvertToF64", 18);
          sm_string *message =
            sm_new_fstring_at(sms_heap, "Cannot convert string to f64: %s", str_content);
          return (((sm_object *)sm_new_error_from_expr(title, message, sme, NULL)));
        }
        return (((sm_object *)sm_new_f64(value)));
      }
      default: {
        sm_symbol *title   = sm_new_symbol("cannotConvertToF64", 18);
        sm_string *message = sm_new_fstring_at(sms_heap, "Cannot convert object of type %s to f64.",
                                               sm_type_name(fromObj->my_type));
        return (((sm_object *)sm_new_error_from_expr(title, message, sme, NULL)));
      }
      }
    }
    }
  }
  }
  return input;
}
