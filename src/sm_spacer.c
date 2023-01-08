// This project is licensed under the BSD 2 clause license. See LICENSE.txt for more information.

#include "sms.h"

// overwrites an existing object
sm_spacer *sm_new_spacer(void *trash, unsigned int size) {
  // Size must be more than sizeof(sm_spacer)
  // Size sets the whole obj size manually
  sm_spacer *new_spacer = (sm_spacer *)trash;
  new_spacer->my_type   = sm_spacer_type;
  new_spacer->size      = size;
  return new_spacer;
}
