// This project is licensed under the BSD 2 clause license. See LICENSE.txt for more information.

#include "sms.h"

sm_symbol *sm_new_symbol(sm_string *sym_name) {
  sm_symbol *sym = (sm_symbol *)sm_malloc(sizeof(sm_symbol));
  sym->my_type   = sm_symbol_type;
  sym->name      = sym_name;
  return sym;
}

sm_string *sm_symbol_to_string(sm_symbol *self) {
  return sm_new_string(self->name->size, &(self->name->content));
}
