#include "sms.h"

sm_symbol *sm_new_symbol(sm_string *sym_name){
  sm_symbol *sym = (sm_symbol*)sm_malloc(sizeof(sm_symbol));
  sym->my_type = sm_symbol_type;
  sym->name=sym_name;
  return sym;
}