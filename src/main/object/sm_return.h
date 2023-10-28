// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

/// This structure forms a return statement
typedef struct sm_return {
  unsigned short int my_type;
  sm_object         *address;
} sm_return;

/// Construct a new return object
sm_return *sm_new_return(sm_object *address);
/// Print this return object to string buffer
unsigned int sm_return_sprint(sm_return *self, char *buffer, bool fake);
