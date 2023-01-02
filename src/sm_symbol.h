typedef struct sm_symbol {
  enum object_type my_type;
  sm_string       *name;
} sm_symbol;
sm_symbol *sm_new_symbol(sm_string *sym_name);
