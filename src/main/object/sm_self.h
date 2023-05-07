// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

typedef struct sm_self {
  int           my_type;
  struct sm_cx *context;
} sm_self;

struct sm_self *sm_new_self(struct sm_cx *context);
int             sm_self_sprint(sm_self *self, char *buffer, bool fake);
