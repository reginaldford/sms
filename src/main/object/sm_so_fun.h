// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

// @brief  Shared object function structure
typedef struct sm_so_fun {
  uint32_t my_type;
  // TODO: optionally typed, and defaults
  // sm_xpr* types; // null if not specified.
  void *function;
} sm_so_fun;

/// Construct a new shared object function, pointing to a function
sm_so_fun *sm_new_so_fun(void *fun);
/// Print to so_fun buffer
uint32_t sm_so_fun_sprint(sm_so_fun *self, char *buffer, bool fake);
