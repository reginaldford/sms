// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"

// This function overwrites an object and leaves a pointer to the new location.
sm_pointer *sm_new_pointer(sm_heap *origin, sm_object *old_object, sm_object *new_address) {
  sm_pointer *ptr = (sm_pointer *)old_object;
  ptr->my_type    = SM_POINTER_TYPE;
  ptr->address    = (uint32_t)((uint64_t)new_address - (uint64_t)origin);
  return ptr;
}

sm_object *sm_pointer_deref(sm_pointer *p, sm_heap *h) {
  return (sm_object *)((uint64_t)h + (uint64_t)(p->address));
}
