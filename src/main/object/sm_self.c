// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"

struct sm_self *sm_new_self(struct sm_cx *context) {
  sm_self *new_self = sm_malloc(sizeof(sm_self));
  new_self->my_type = SM_SELF_TYPE;
  new_self->context = context;
  return new_self;
}

int sm_self_sprint(sm_self *self, char *buffer, bool fake) {
  if (!fake)
    sm_strncpy(buffer, "self", 4);
  return 4;
}
