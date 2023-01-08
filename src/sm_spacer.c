// This project is licensed under the BSD 2 clause license. See LICENSE.txt for more information.

#include "sms.h"

sm_spacer *sm_new_spacer(unsigned int size) {
  // size must be more than sizeof(sm_spacer)
  // Size sets the whole obj size, manually
  sm_spacer *new_spacer = sm_malloc(size);
  new_spacer->my_type   = sm_spacer_type;
  new_spacer->size      = size;
  return new_spacer;
}
