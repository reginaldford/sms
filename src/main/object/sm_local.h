// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

/// Local variable, specifying the index of the stack frame tuple to reference
typedef struct sm_local {
  int16_t           my_type;
  struct sm_string *name;
  uint16_t          index;
} sm_local;

/// Construct a new local
sm_local *sm_new_local(uint16_t index, sm_string *name);
/// Takes the expression (obj) and changes symbols to locals if they match parameters of fun
sm_object *sm_localize(sm_object *obj, sm_fun *fun);
/// Change local variables into symbols with the same name
sm_object *sm_unlocalize(sm_object *obj);
/// If !fake, print this local variable to string buffer. Return length regardlessly.
uint32_t sm_local_sprint(sm_local *l, char *buffer, bool fake);
