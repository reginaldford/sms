// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"

// This function overwrites an object and leaves a pointer to the new location.
sm_pointer *sm_new_pointer(sm_heap *origin, sm_object *old_object, sm_object *new_address) {
  if (((intptr_t)old_object) & 7) {
    fprintf(stderr, "bad call to sm_new_pointer\n");
    exit(1);
  }
  sm_pointer *ptr = (sm_pointer *)old_object;
  ptr->my_type    = SM_POINTER_TYPE;
  ptr->address    = (uint32_t)((uint64_t)new_address - (uint64_t)origin);
  return ptr;
}
