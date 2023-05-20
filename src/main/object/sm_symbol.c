// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"

// Just create a symbol
sm_symbol *sm_new_symbol_manual(sm_string *sym_name) {
  sm_symbol *sym = (sm_symbol *)sm_malloc(sizeof(sm_symbol));
  sym->my_type   = SM_SYMBOL_TYPE;
  sym->name      = sym_name;
  return sym;
}

// Returns a new symbol
sm_symbol *sm_new_symbol(sm_string *sym_name) {
  if (strncmp(&sym_name->content, "true", 4) == 0)
    return sm_global_true(NULL);
  else if (strncmp(&sym_name->content, "false", 5) == 0)
    return sm_global_false(NULL);
  else
    return sm_new_symbol_manual(sym_name);
}

sm_string *sm_symbol_to_string(sm_symbol *self) {
  return sm_new_string(self->name->size, &(self->name->content));
}

unsigned int sm_symbol_sprint(sm_symbol *self, char *buffer, bool fake) {
  if (!fake)
    sm_strncpy(buffer, &(self->name->content), self->name->size);
  return self->name->size;
}
