// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"

// New sm_return, a pointer to an object
// Signifying that a then-block is to return this value without continuing.
// Functions will unwrap this object.
sm_return *sm_new_return(sm_object *address) {
  sm_return *smr = sm_malloc(sizeof(sm_return));
  smr->my_type   = SM_RETURN_TYPE;
  smr->address   = address;
  return smr;
}

// New string with description of this sm_return
sm_string *sm_return_to_string(sm_return *return_obj) {
  const unsigned int final_len = sm_return_sprint(return_obj, NULL, true);
  sm_string         *new_str   = sm_new_string_manual(final_len);
  sm_return_sprint(return_obj, &(new_str->content), false);
  (&new_str->content)[final_len] = '\0';
  return new_str;
}

// Print to c string buffer a description of this sm_return
unsigned int sm_return_sprint(sm_return *return_obj, char *buffer, bool fake) {
  if (!fake)
    snprintf(buffer, 8, "return(");
  int cursor = 7;
  cursor += sm_object_sprint(return_obj->address, &(buffer[cursor]), fake);
  if (!fake)
    buffer[cursor] = ')';
  cursor++;
  return cursor;
}
