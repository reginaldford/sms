// The following file is provided under the BSD 2-clause license. For more info, read LICENSE.txt.

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

unsigned int sm_symbol_sprint(sm_symbol *self, char *buffer) {
  sm_strncpy(buffer, &(self->name->content), self->name->size);
  return self->name->size;
}

unsigned int sm_symbol_to_string_len(sm_symbol *self) { return self->name->size; }
