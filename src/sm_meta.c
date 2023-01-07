#include "sms.h"

sm_meta *sm_new_meta(unsigned int meta_level, sm_object *address) {
  sm_meta *smm    = sm_malloc(sizeof(sm_meta));
  smm->my_type    = sm_meta_type;
  smm->meta_level = meta_level;
  smm->address    = address;
  return smm;
}

sm_string *meta_to_string(sm_meta *meta) {
  sm_string *object_string = sm_object_to_string(meta->address);
  return sm_concat_strings(sm_new_string(1, ":"), object_string);
}
