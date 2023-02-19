// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

// Local variable, specifying the index of the stack frame array
typedef struct sm_local {
  short int          my_type;
  struct sm_string  *name;
  unsigned short int index;
} sm_local;

sm_local    *sm_new_local(unsigned short int index, sm_string *name);
sm_expr     *sm_localize_expr(sm_expr *sme, sm_fun *fun);
unsigned int sm_local_sprint(sm_local *l, char *buffer, bool fake);
