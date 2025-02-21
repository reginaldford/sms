// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

// @brief  Shared object function structure
typedef struct sm_so_fun_call {
  uint32_t   my_type;
  sm_so_fun *f;
  sm_object *input;
} sm_so_fun_call;

/// Construct a new shared object function
sm_so_fun_call *sm_new_so_fun_call(void *fun, uint32_t num_inputs);
/// Print to so_fun_call buffer
uint32_t sm_so_fun_call_sprint(sm_so_fun_call *self, char *buffer, bool fake);
