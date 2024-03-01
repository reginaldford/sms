// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"

// New meta, a pointer to an object
sm_meta *sm_new_meta(sm_object *address, sm_cx *context) {
  sm_meta *smm = sm_malloc(sms_heap, sizeof(sm_meta));
  smm->my_type = SM_META_TYPE;
  smm->address = address;
  smm->scope   = context;
  return smm;
}

// New string with description of this meta
sm_string *sm_meta_to_string(sm_meta *meta) {
  const uint32_t final_len = sm_meta_sprint(meta, NULL, true);
  sm_string     *new_str   = sm_new_string_manual(final_len);
  sm_meta_sprint(meta, &(new_str->content), false);
  (&new_str->content)[final_len] = '\0';
  return new_str;
}

// Print to c string buffer a description of this meta
uint32_t sm_meta_sprint(sm_meta *meta, char *buffer, bool fake) {
  if (!fake)
    buffer[0] = ':';
  if (meta->address->my_type == SM_EXPR_TYPE) {
    if (sm_is_infix(((sm_expr *)meta->address)->op)) {
      if (((sm_expr *)meta->address)->size < 3) {
        if (!fake)
          buffer[1] = '(';
        uint32_t len = sm_object_sprint(meta->address, &(buffer[2]), fake);
        if (!fake)
          buffer[2 + len] = ')';
        return len + 3;
      }
    }
  }
  uint32_t len = sm_object_sprint(meta->address, &(buffer[1]), fake);
  return len + 1;
}
