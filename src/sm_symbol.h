// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

typedef struct sm_symbol {
  short int  my_type;
  sm_string *name;
} sm_symbol;

sm_symbol   *sm_new_symbol(sm_string *sym_name);
sm_string   *sm_symbol_to_string(sm_symbol *self);
unsigned int sm_symbol_sprint(sm_symbol *self, char *buffer, bool fake);
