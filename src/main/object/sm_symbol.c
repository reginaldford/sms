// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"

extern struct sm_heap *sms_heap;
extern struct sm_heap *sms_symbol_heap;
extern struct sm_heap *sms_symbol_name_heap;

// Just create a symbol with the given name
sm_symbol *sm_new_symbol_manual(char *name, int name_len) {
  sm_symbol *sym = (sm_symbol *)sm_malloc(sms_symbol_heap, sizeof(sm_symbol));
  sym->my_type   = SM_SYMBOL_TYPE;
  sym->name      = sm_new_string_at(sms_symbol_name_heap, name_len, name);
  return sym;
}

// Returns a new symbol unless it's true or false
sm_symbol *sm_new_symbol(char *sym_name, int len) {
  if (strncmp(sym_name, "true", 4) == 0)
    return sms_true;
  else if (strncmp(sym_name, "false", 5) == 0)
    return sms_false;
  else
    return sm_new_symbol_manual(sym_name, len);
}

sm_string *sm_symbol_to_string(sm_symbol *self) { return self->name; }

uint32_t sm_symbol_sprint(sm_symbol *self, char *buffer, bool fake) {
  if (!fake)
    sm_strncpy(buffer, &(self->name->content), self->name->size);
  return self->name->size;
}
