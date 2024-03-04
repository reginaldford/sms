// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

/// Object for a symbol which references a value in a context if evaluated
typedef struct sm_symbol {
  int16_t    my_type;
  sm_string *name;
  sm_string *crypt_id;
} sm_symbol;

/// Encrypts the id of the symbol and returns a string
/// The string will be stored in sms_symbol_name_heap
/// Acts as a nickname for the symbol, speeding up sms mapped tries
sm_string *sm_symbol_encrypt_id(sm_symbol *sym);
/// Decrypt the id of the symbol
/// id is the array index of the symbol in the symbol heap
sm_symbol *sm_symbol_decrypt_id(char *crypt_id, int crypt_id_len);
/// Returns a new or existing symbol from the symbol heap
sm_symbol *sm_new_symbol(char *name, int name_len);
/// Returns the symbol name
sm_string *sm_symbol_to_string(sm_symbol *self);
/// Print the symbol name to string buffer
uint32_t sm_symbol_sprint(sm_symbol *self, char *buffer, bool fake);
