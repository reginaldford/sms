#include "sms.h"

sm_key_value *sm_new_key_value(sm_string *key, sm_object *value) {
  sm_key_value *new_kvo = sm_malloc(sizeof(sm_key_value));
  new_kvo->my_type      = sm_key_value_type;
  new_kvo->name         = key;
  new_kvo->value        = value;
  return new_kvo;
}
