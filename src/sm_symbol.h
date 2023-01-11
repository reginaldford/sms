// This project is licensed under the BSD 2 clause license. See LICENSE.txt for more information.

typedef struct sm_symbol {
  enum sm_object_type my_type;
  sm_string          *name;
} sm_symbol;

sm_symbol *sm_new_symbol(sm_string *sym_name);
sm_string *sm_symbol_to_string(sm_symbol *self);
