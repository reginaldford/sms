// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "sms.h"

sm_symbol *sm_new_symbol(sm_string *sym_name) {
  sm_symbol *sym = (sm_symbol *)sm_malloc(sizeof(sm_symbol));
  sym->my_type   = SM_SYMBOL_TYPE;
  sym->name      = sym_name;
  return sym;
}

sm_string *sm_symbol_to_string(sm_symbol *self) {
  return sm_new_string(self->name->size, &(self->name->content));
}

unsigned int sm_symbol_sprint(sm_symbol *self, char *buffer, bool fake) {
  if (!fake)
    sm_strncpy(buffer, &(self->name->content), self->name->size);
  return self->name->size;
}
