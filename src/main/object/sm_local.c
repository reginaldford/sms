// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"

// A local variable stores the index of the 'stack frame' array.
sm_local *sm_new_local(uint16_t index, sm_string *name) {
  sm_local *new_local = sm_malloc(sizeof(sm_local));
  new_local->my_type  = SM_LOCAL_TYPE;
  new_local->index    = index;
  new_local->name     = name;
  return new_local;
}

// If this symbol matches a parameter of the function, return which.
// Else, return -1
int sym_matches_param(sm_symbol *sym, sm_fun *fun) {
  for (uint16_t i = 0; i < fun->num_params; i++) {
    sm_fun_param *param       = sm_fun_get_param(fun, i);
    sm_string    *param_str   = param->name;
    char         *param_cstr  = &param_str->content;
    sm_string    *symbol_str  = sym->name;
    char         *symbol_cstr = &symbol_str->content;
    if (strcmp(param_cstr, symbol_cstr) == 0)
      return i;
  }
  return -1;
}

// Replace symbols in the expression with locals for function definition
// This way, local variables use stack frame references.
sm_object *sm_localize(sm_object *obj, sm_fun *fun) {
  if (obj->my_type != SM_CX_TYPE)
    obj = sm_copy(obj);
  if (obj->my_type == SM_EXPR_TYPE) {
    sm_expr *sme = (sm_expr *)obj;

    if (sme->op == SM_ASSIGN_EXPR) {
      int which_param = sym_matches_param((sm_symbol *)sm_expr_get_arg(sme, 0), fun);
      if (which_param != -1) {
        sme->op        = SM_ASSIGN_LOCAL_EXPR;
        sm_symbol *sym = (sm_symbol *)sm_expr_get_arg(sme, 0);
        sm_expr_set_arg(sme, 0, (sm_object *)sm_new_local(which_param, sym->name));
      }
    }

    for (uint32_t i = 0; i < sme->size; i++) {
      sm_object *current_obj   = sm_expr_get_arg(sme, i);
      sm_object *processed_obj = sm_localize(current_obj, fun);
      sm_expr_set_arg(sme, i, processed_obj);
    }
  } else if (obj->my_type == SM_SYMBOL_TYPE) {
    sm_symbol *sym         = (sm_symbol *)obj;
    int        which_param = sym_matches_param(sym, fun);
    if (which_param != -1)
      return (sm_object *)sm_new_local(which_param, sym->name);
  }
  return obj;
}

// Replace local variables with symbol references in the expression
sm_object *sm_unlocalize(sm_object *obj) {
  if (obj->my_type != SM_CX_TYPE)
    obj = sm_copy(obj);

  if (obj->my_type == SM_EXPR_TYPE) {
    sm_expr *sme = (sm_expr *)obj;
    if (sme->op == SM_ASSIGN_LOCAL_EXPR) {
      sme->op         = SM_ASSIGN_EXPR;
      sm_local *local = (sm_local *)sm_expr_get_arg(sme, 0);
      return (sm_object *)sm_new_symbol(&(local->name->content), local->name->size);
    }
    for (uint32_t i = 0; i < sme->size; i++) {
      sm_object *current_obj   = sm_expr_get_arg(sme, i);
      sm_object *processed_obj = sm_unlocalize(current_obj);
      sm_expr_set_arg(sme, i, processed_obj);
    }
  } else if (obj->my_type == SM_LOCAL_TYPE) {
    sm_local  *local  = (sm_local *)obj;
    sm_symbol *symbol = sm_new_symbol(&(local->name->content), local->name->size);
    return (sm_object *)symbol;
  }
  return obj;
}

// Print to string buffer a description of this local variable name
uint32_t sm_local_sprint(sm_local *l, char *buffer, bool fake) {
  if (!fake)
    sm_strncpy(buffer, &(l->name->content), l->name->size);
  return l->name->size;
}
