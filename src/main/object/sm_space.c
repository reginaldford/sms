// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"

// General purpose space of memory
sm_space *sm_new_space(uint32_t size) {
  sm_space *new_space = sm_malloc(sizeof(sm_space) + (size << 2));
  new_space->my_type  = SM_SPACE_TYPE;
  new_space->size     = size;
  return new_space;
}
