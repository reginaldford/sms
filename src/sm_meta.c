// The following file is provided under the BSD 2-clause license. For more info, read LICENSE.txt.

#include "sms.h"

// New meta, a pointer to an object
sm_meta *sm_new_meta(sm_object *address, sm_context *context) {
  sm_meta *smm = sm_malloc(sizeof(sm_meta));
  smm->my_type = sm_meta_type;
  smm->address = address;
  smm->scope   = context;
  return smm;
}

// New string with description of this meta
sm_string *sm_meta_to_string(sm_meta *meta) {
  const unsigned int final_len = sm_meta_to_string_len(meta);
  sm_string         *new_str   = sm_new_string(final_len, "");
  sm_meta_sprint(meta, &(new_str->content));
  (&new_str->content)[final_len] = '\0';
  return new_str;
}

// Print to c string buffer a description of this meta
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

// Return the hypothetical length of the result of sm_meta_to_string(this)
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
