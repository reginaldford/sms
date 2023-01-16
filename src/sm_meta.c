// The following file is provided under the BSD 2-clause license. For more info, read LICENSE.txt.
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
        result_string = sm_string_add_recycle(sm_new_string(2, ":("), result_string);
        return sm_string_add_recycle(result_string, sm_new_string(1, ")"));
      }
    }
  }
  return sm_string_add_recycle(sm_new_string(1, ":"), result_string);
}

unsigned int sm_meta_sprint(sm_meta *meta, char *buffer) {
  buffer[0] = ':';
  if (meta->address->my_type == sm_expr_type) {
    if (sm_is_infix(((sm_expr *)meta->address)->op)) {
      if (((sm_expr *)meta->address)->size < 3) {
        buffer[1]        = '(';
        unsigned int len = sm_object_sprint(meta->address, &(buffer[2]));
        buffer[2 + len]  = ')';
        return len + 3;
      }
    }
  }
  unsigned int len = sm_object_sprint(meta->address, &(buffer[1]));
  return len + 1;
}

unsigned int sm_meta_to_string_len(sm_meta *self) {
  unsigned int result = sm_object_to_string_len(self->address);
  if (self->address->my_type == sm_expr_type) {
    if (sm_is_infix(((sm_expr *)self->address)->op)) {
      if (((sm_expr *)self->address)->size < 3) {
        return result + 3; // adding ":(" and ")"
      }
    }
  }
  return result + 1; // just adding ":"
}
