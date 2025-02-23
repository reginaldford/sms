// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"

/// Construct a new shared object function
sm_so_fun *sm_new_so_fun(void *fn) {
  struct sm_so_fun *new_fun = sm_malloc(sizeof(sm_so_fun));
  new_fun->my_type          = SM_SO_FUN_TYPE;
  new_fun->function         = fn;
  return new_fun;
}

// If fake is false,
// Prints to to_str a string describing the SO function
// Returns the length regardless
uint32_t sm_so_fun_sprint(sm_so_fun *self, char *to_str, bool fake) {
  if (!fake)
    return sprintf(to_str, "(SO_FUN@%p)", self->function);
  else {
    char tempBuffer[32];
    return sprintf(tempBuffer, "(SO_FUN@%p)", self->function);
  }
}
