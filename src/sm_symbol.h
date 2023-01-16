// The following file is provided under the BSD 2-clause license. For more info, read LICENSE.txt.

typedef struct sm_symbol {
  short int  my_type;
  sm_string *name;
} sm_symbol;

sm_symbol   *sm_new_symbol(sm_string *sym_name);
sm_string   *sm_symbol_to_string(sm_symbol *self);
unsigned int sm_symbol_sprint(sm_symbol *self, char *buffer);
unsigned int sm_symbol_to_string_len(sm_symbol *self);
