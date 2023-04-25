// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

typedef struct sm_meta {
  int        my_type;
  sm_object *address;
  sm_cx     *scope;
  sm_cx     *data;
} sm_meta;

sm_meta     *sm_new_meta(sm_object *address, sm_cx *context);
sm_string   *sm_meta_to_string(sm_meta *meta);
unsigned int sm_meta_sprint(sm_meta *self, char *buffer, bool fake);
