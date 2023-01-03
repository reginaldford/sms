#include "sms.h"

sm_pointer * sm_new_pointer(sm_object * old_object, sm_object * new_address)
{
  sm_pointer * ptr = (sm_pointer*) old_object;
  ptr->my_type = sm_pointer_type;
  ptr->address = new_address;
  return ptr;
}
