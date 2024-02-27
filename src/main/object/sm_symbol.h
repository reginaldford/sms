// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

/// Object for a symbol which references a value in a context if evaluated
typedef struct sm_symbol {
  int16_t    my_type;
  sm_string *name;
} sm_symbol;

/// Create a new symbol without checking for true/false singletons
sm_symbol *sm_new_symbol_manual(char * name, int name_len);
/// Create a new symbol without checking for true/false singletons
sm_symbol *sm_new_symbol_manual_at(char * name, int name_len);
/// Create a new symbol. If sym_name is "true" or "false", return correlating singletons
sm_symbol *sm_new_symbol(char * name, int name_len);
/// New symbol at specific heap. If sym_name is "true" or "false", return correlating singletons
sm_symbol *sm_new_symbol_at(sm_heap* heap,char * name, int name_len);
/// Returns the symbol name
sm_string *sm_symbol_to_string(sm_symbol *self);
/// Print the symbol name to string buffer
uint32_t sm_symbol_sprint(sm_symbol *self, char *buffer, bool fake);
