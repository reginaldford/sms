// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

typedef struct sm_return {
  unsigned short int my_type;
  sm_object         *address;
} sm_return;

sm_return   *sm_new_return(sm_object *address);
sm_string   *sm_return_to_string(sm_return *return_obj);
unsigned int sm_return_sprint(sm_return *self, char *buffer, bool fake);
