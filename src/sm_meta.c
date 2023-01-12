// This project is licensed under the BSD 2 clause license. See LICENSE.txt for more information.
#include "sms.h"

sm_meta *sm_new_meta(sm_object *address) {
  sm_meta *smm = sm_malloc(sizeof(sm_meta));
  smm->my_type = sm_meta_type;
  smm->address = address;
  return smm;
}

sm_string *sm_meta_to_string(sm_meta *meta) {
  sm_string *result_string = sm_object_to_string(meta->address);
  if (meta->address->my_type == sm_expr_type) {
    if (sm_is_infix(((sm_expr *)meta->address)->op)) {
      if (((sm_expr *)meta->address)->size < 3) {
        result_string = sm_string_add_recycle(sm_new_string(3, ":( "), result_string);
        result_string = sm_string_add_recycle(result_string, sm_new_string(2, " )"));
        return result_string;
      }
    }
  }
  result_string = sm_string_add_recycle(sm_new_string(1, ":"), result_string);
  return result_string;
}
