// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"

extern sm_heap *sms_heap;

// Create a new shard object
sm_so *sm_new_so(void *handle) {
  struct sm_so *new_so = sm_malloc(sizeof(sm_so));
  new_so->my_type      = SM_SO_TYPE;
  new_so->handle       = handle;
  return new_so;
}

// If fake is false,
// Prints to to_str a string describing the SO
// Returns the length regardless
uint32_t sm_so_sprint(sm_so *self, char *to_str, bool fake) {
  if (!fake)
    return sprintf(to_str, "(SO@%p)", self->handle);
  else {
    char tempBuffer[32];
    return sprintf(tempBuffer, "(SO@%p)", self->handle);
  }
}

// Returns whether two SOs match handle ptr
bool sm_so_is_equal(sm_so *so1, sm_so *so2) { return so1->handle == so2->handle; }


///
///
// Create a new shard object
sm_ff_fun *sm_new_ff_fun(void *handle) {
  struct sm_ff_fun *new_ff_fun = sm_malloc(sizeof(sm_ff_fun));
  new_ff_fun->my_type          = SM_FF_FUN_TYPE;
  new_ff_fun->fun              = handle;
  return new_ff_fun;
}

// If fake is false,
// Prints to to_str a string describing the ff_fun
// Returns the length regardless
uint32_t sm_ff_fun_sprint(sm_ff_fun *self, char *to_str, bool fake) {
  if (!fake)
    return sprintf(to_str, "(ff_fun@%p)", self->fun);
  else {
    char tempBuffer[32];
    return sprintf(tempBuffer, "(ff_fun@%p)", self->fun);
  }
}

// Returns whether two ff_funs match handle ptr
bool sm_ff_fun_is_equal(sm_ff_fun *ff_fun1, sm_ff_fun *ff_fun2) {
  return ff_fun1->fun == ff_fun2->fun;
}


// Create a new shard object
struct sm_ff_sig *sm_new_ff_sig(ffi_cif cif) {
  struct sm_ff_sig *new_ff_sig = sm_malloc(sizeof(struct sm_ff_sig));
  new_ff_sig->my_type          = SM_FF_SIG_TYPE;
  new_ff_sig->cif              = cif;
  return new_ff_sig;
}

// If fake is false,
// Prints to to_str a string describing the ff_sig
// Returns the length regardless
uint32_t sm_ff_sig_sprint(sm_ff_sig *self, char *to_str, bool fake) {
  if (!fake)
    return sprintf(to_str, "(ff_sig@%p)", &self->cif);
  else {
    char tempBuffer[32];
    return sprintf(tempBuffer, "(ff_sig@%p)", &self->cif);
  }
}

// Returns whether two ff_sigs match handle ptr
// bool sm_ff_sig_is_equal(sm_ff_sig *ff_sig1, sm_ff_sig *ff_sig2) { return ((void*)ff_sig1->cif) ==
// ((void*)ff_sig2->cif); }
