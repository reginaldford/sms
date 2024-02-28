// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"

extern struct sm_heap *sms_heap;
extern struct sm_heap *sms_symbol_heap;
extern struct sm_heap *sms_symbol_name_heap;

// Returns a new symbol unless it's true or false
sm_symbol *sm_new_symbol(char *name, int len) {
  sm_symbol *sym = (sm_symbol *)sm_malloc(sms_symbol_heap, sizeof(sm_symbol));
  sym->my_type   = SM_SYMBOL_TYPE;
  sym->name      = sm_new_string_at(sms_symbol_name_heap, len, name);
  return sym;
}

sm_string *sm_symbol_to_string(sm_symbol *self) { return self->name; }

uint32_t sm_symbol_sprint(sm_symbol *self, char *buffer, bool fake) {
  if (!fake)
    sm_strncpy(buffer, &(self->name->content), self->name->size);
  return self->name->size;
}
