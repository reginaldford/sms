// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"

// New sm_return, a pointer to an object
// Signifying that a then-block is to return this value without continuing.
// Functions will unwrap this object.
sm_return *sm_new_return(sm_object *address) {
  sm_return *smr = sm_malloc(sms_heap, sizeof(sm_return));
  smr->my_type   = SM_RETURN_TYPE;
  smr->address   = address;
  return smr;
}

// Print to c string buffer a description of this sm_return
uint32_t sm_return_sprint(sm_return *return_obj, char *buffer, bool fake) {
  if (!fake)
    snprintf(buffer, 8, "return(");
  int cursor = 7;
  cursor += sm_object_sprint(return_obj->address, &(buffer[cursor]), fake);
  if (!fake)
    buffer[cursor] = ')';
  cursor++;
  return cursor;
}
