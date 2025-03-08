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
