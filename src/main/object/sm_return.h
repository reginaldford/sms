// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

/// This structure forms a return statement
typedef struct sm_return {
  uint32_t   my_type;
  sm_object *address;
} sm_return;

/// Construct a new return object
sm_return *sm_new_return(sm_object *address);
/// Print this return object to string buffer
uint32_t sm_return_sprint(sm_return *self, char *buffer, bool fake);
