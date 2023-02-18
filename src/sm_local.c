// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "sms.h"

sm_local *sm_new_local(unsigned short int index, sm_string *name) {
  sm_local *new_local = sm_malloc(sizeof(sm_local));
  new_local->my_type  = sm_local_type;
  new_local->index    = index;
  new_local->name     = name;
  return new_local;
}

// Replace symbols in the expression with locals for function definition
// This way, local variables use stack frame references.
sm_expr *sm_localize_expr(sm_expr *sme, sm_fun *fun) {
  if (sme->my_type == sm_expr_type) {
    for (unsigned int i = 0; i < sme->size; i++) {
      sm_object *current_obj = sm_expr_get_arg(sme, i);
      if (current_obj->my_type == sm_symbol_type) {
        sm_symbol *sym = (sm_symbol *)current_obj;
        for (unsigned short int i = 0; i < fun->num_params; i++) {
          if (strcmp(&(sm_fun_get_param(fun, i)->name->content), &(sym->name->content)) == 0) {
            sme = sm_set_expr_arg(sme, i, (sm_object *)sm_new_local(i, sym->name));
          }
        }
      } else if (current_obj->my_type == sm_expr_type) {
        sme = sm_set_expr_arg(sme, i, (sm_object *)sm_localize_expr((sm_expr *)current_obj, fun));
      }
    }
  }
  return sme;
}

// Print to string buffer a description of this context entry
unsigned int sm_local_sprint(sm_local *l, char *buffer, bool fake) {
  if (!fake)
    sm_strncpy(buffer, &(l->name->content), l->name->size);
  return l->name->size;
}
