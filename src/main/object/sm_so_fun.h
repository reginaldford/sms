// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

// @brief  Shared object function structure
typedef struct sm_so_fun {
  uint32_t my_type;
  uint32_t num_inputs;
  // TODO: optionally type
  // sm_xpr* types; // null if not specified.
  void *function;
} sm_so_fun;

/// Construct a new shared object function
sm_so_fun *sm_new_so_fun(void *fun, uint32_t num_inputs);
/// Print to so_fun buffer
uint32_t sm_so_fun_sprint(sm_so_fun *self, char *buffer, bool fake);
