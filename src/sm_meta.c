#include "sms.h"

sm_meta *sm_new_meta(sm_object *address) {
  sm_meta *smm = sm_malloc(sizeof(sm_meta));
  smm->my_type = sm_meta_type;
  smm->address = address;
  return smm;
}

sm_string *sm_meta_to_string(sm_meta *meta) {
  sm_string *result_string = sm_object_to_string(meta->address);
  result_string            = sm_concat_strings(sm_new_string(1, ":"), result_string);
  return result_string;
}
