// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"

extern struct sm_heap *sms_heap;
extern struct sm_heap *sms_symbol_heap;
extern struct sm_heap *sms_symbol_name_heap;
extern uint32_t        sms_num_symbols;

// Base 64 Encode the id of the symbol
sm_string *sm_symbol_encode_id(sm_symbol *sym) {
  int32_t sym_id = sym - (sm_symbol *)sms_symbol_heap->storage;
  // Turn sym_id into hex
  char    output_str[7];
  uint8_t out_str_len = 0;
  while (sym_id) {
    output_str[out_str_len++] = (char)(sym_id & 63);
    sym_id                    = sym_id >> 6;
  }
  output_str[out_str_len] = '\0';
  return sm_new_string_at(sms_symbol_name_heap, out_str_len, output_str);
}

// Returns a new or existing symbol from the symbol heap
sm_symbol *sm_new_symbol(char *name, int len) {
  sm_symbol *current_sym = (sm_symbol *)sms_symbol_heap->storage;
  // Looking for the existing symbol
  for (uint32_t i = 0; i < sms_num_symbols; i++) {
    uint32_t current_sym_len = current_sym->name->size;
    if (len == current_sym_len && !strncmp(name, &current_sym->name->content, len))
      return current_sym;
    current_sym = current_sym + 1;
  }
  // Was not found. Creating a new symbol
  sm_symbol *sym = (sm_symbol *)sm_malloc_at(sms_symbol_heap, sizeof(sm_symbol));
  sym->my_type   = SM_SYMBOL_TYPE;
  sym->name      = sm_new_string_at(sms_symbol_name_heap, len, name);
  sym->code_id   = sm_symbol_encode_id(sym);
  sms_num_symbols++;
  return sym;
}

sm_string *sm_symbol_to_string(sm_symbol *self) { return self->name; }

uint32_t sm_symbol_sprint(sm_symbol *self, char *buffer, bool fake) {
  if (!fake)
    sm_strncpy(buffer, &(self->name->content), self->name->size);
  return self->name->size;
}
