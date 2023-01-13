// The following file is provided under the BSD 2-clause license. For more info, read LICENSE.txt.
#include "sms.h"

// This function overwrites an object and leaves a pointer to the new location.
sm_pointer *sm_new_pointer(sm_object *old_object, sm_object *new_address) {
  sm_pointer *ptr = (sm_pointer *)old_object;
  ptr->my_type    = sm_pointer_type;
  ptr->address    = new_address;
  return ptr;
}
