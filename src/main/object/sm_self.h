// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

/// This structure represents a self statement
typedef struct sm_self {
  int32_t       my_type;
  struct sm_cx *context;
} sm_self;

/// Create a new self object
struct sm_self *sm_new_self();
/// Print this self to string buffer
int32_t sm_self_sprint(sm_self *self, char *buffer, bool fake);
