// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

/// Object for a symbol which references a value in a context if evaluated
typedef struct sm_symbol {
  int32_t    my_type;
  sm_string *name;
  sm_string *code_id;
} sm_symbol;

/// Encodes the id of the symbol and returns a string
/// The string will be stored in sms_symbol_name_heap
/// Acts as a nickname for the symbol, speeding up sms mapped tries
sm_string *sm_symbol_encode_id(sm_symbol *sym);
/// Decode the id of the symbol
/// id is the tuple index of the symbol in the symbol heap
sm_symbol *sm_symbol_decode_id(char *code_id, int code_id_len);
/// Returns a new or existing symbol from the symbol heap
sm_symbol *sm_new_symbol(char *name, int name_len);
/// Returns the symbol name
sm_string *sm_symbol_to_string(sm_symbol *self);
/// Print the symbol name to string buffer
uint32_t sm_symbol_sprint(sm_symbol *self, char *buffer, bool fake);
