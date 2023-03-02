// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "sms.h"

// This function overwrites an object and leaves a pointer to the new location.
sm_pointer *sm_new_pointer(sm_object *old_object, sm_object *new_address) {
  sm_pointer *ptr = (sm_pointer *)old_object;
  ptr->my_type    = sm_pointer_type;
  ptr->address    = new_address;
  return ptr;
}
