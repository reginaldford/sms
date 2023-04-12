// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"

// A local variable stores the index of the 'stack frame' array.
sm_local *sm_new_local(unsigned short int index, sm_string *name) {
  sm_local *new_local = sm_malloc(sizeof(sm_local));
  new_local->my_type  = SM_LOCAL_TYPE;
  new_local->index    = index;
  new_local->name     = name;
  return new_local;
}

// Replace symbols in the expression with locals for function definition
// This way, local variables use stack frame references.
sm_object *sm_localize(sm_object *obj, sm_fun *fun) {
  if (obj->my_type == SM_EXPR_TYPE) {
    sm_expr *sme = (sm_expr *)obj;
    for (unsigned int i = 0; i < sme->size; i++) {
      sm_object *current_obj   = sm_expr_get_arg(sme, i);
      sm_object *processed_obj = sm_localize(current_obj, fun);
      sm_expr_set_arg(sme, i, processed_obj);
    }
  } else if (obj->my_type == SM_SYMBOL_TYPE) {
    sm_symbol *sym = (sm_symbol *)obj;
    for (unsigned short int i = 0; i < fun->num_params; i++) {
      if (strcmp(&(sm_fun_get_param(fun, i)->name->content), &(sym->name->content)) == 0) {
        return (sm_object *)sm_new_local(i, sym->name);
      }
    }
  }
  return obj;
}

// Print to string buffer a description of this local variable name
unsigned int sm_local_sprint(sm_local *l, char *buffer, bool fake) {
  if (!fake)
    sm_strncpy(buffer, &(l->name->content), l->name->size);
  return l->name->size;
}
