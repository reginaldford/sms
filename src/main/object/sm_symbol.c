// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"
#include "../../../submodules/bounce/src/bounce.h"

extern struct sm_heap *sms_heap;
extern struct sm_heap *sms_symbol_heap;
extern struct sm_heap *sms_symbol_name_heap;
extern uint32_t        sms_num_symbols;

// Encrypt the id of the symbol
// id is the array index of the symbol in the symbol heap
sm_string *sm_symbol_encrypt_id(sm_symbol *sym) {
  // static char alpabet[64]="\'
  int                  sym_id       = sym - (sm_symbol *)sms_symbol_heap->storage;
  uint8_t              enc_buffer[] = {0, 0, 0, 0, 0, 0, 0, 0};
  bounceReadFileResult brfr         = bounceReadFile("sms_key");
  if (!brfr.fileExists) {
    printf("key file not found!\n");
    exit(1);
  }
  uint8_t *sms_key = brfr.fileContent;
  // Calculate and store the keySum
  uint32_t sms_ks1 = bounceProcKeySum(sms_key);
  uint32_t sms_ks2 = bounceProcKeySum(sms_key + 128);
  // Invertable Swap table generated from key
  static uint8_t sms_crypt_table[256];
  bounceTableInit(sms_key, sms_crypt_table);
  bounce_encrypt((uint8_t *)&sym_id, 4, sms_key, sms_ks1, sms_ks2, sms_crypt_table, enc_buffer);
  int null_streak;
  for (null_streak = 0; null_streak <= 4; null_streak++)
    if (enc_buffer[4 - null_streak] != 0)
      break;
  int  enc_buffer_used = 4 - null_streak;
  char output_str[32];
  // long is 64 bit, 10 * 6 bit + 4 bits
  long chunk       = (long)*enc_buffer;
  int  out_str_len = ceil(enc_buffer_used * 4.0 / 3.0);
  for (int i = 0; i < out_str_len; i++) {
    char zeroTo63 = (char)(chunk >> i * 6) & 63;
    output_str[i] = sm_node_bit_unindex(zeroTo63);
  }
  output_str[out_str_len] = '\0';
  return sm_new_string_at(sms_symbol_name_heap, out_str_len, output_str);
}

// Returns a new or existing symbol from the symbol heap
sm_symbol *sm_new_symbol(char *name, int len) {
  sm_symbol *current_sym = (sm_symbol *)sms_symbol_heap->storage;
  // Looking for the existing symbol
  for (uint32_t i = 0; i < sms_num_symbols; i++) {
    int current_sym_len = current_sym->name->size;
    if (len == current_sym_len && !strncmp(name, &current_sym->name->content, len))
      return current_sym;
    current_sym = current_sym + 1;
  }
  // Was not found. Creating a new symbol
  sm_symbol *sym = (sm_symbol *)sm_malloc(sms_symbol_heap, sizeof(sm_symbol));
  sym->my_type   = SM_SYMBOL_TYPE;
  sym->name      = sm_new_string_at(sms_symbol_name_heap, len, name);
  sym->crypt_id  = sm_symbol_encrypt_id(sym);
  sms_num_symbols++;
  return sym;
}

sm_string *sm_symbol_to_string(sm_symbol *self) { return self->name; }

uint32_t sm_symbol_sprint(sm_symbol *self, char *buffer, bool fake) {
  if (!fake)
    sm_strncpy(buffer, &(self->name->content), self->name->size);
  return self->name->size;
}
