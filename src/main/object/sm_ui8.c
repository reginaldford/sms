// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"

sm_ui8 *sm_new_ui8(uint8_t value) {
  // Objects must be at least the size of sm_pointer
  struct sm_ui8 *newnum = (sm_ui8 *)sm_malloc(sizeof(sm_ui8));
  newnum->my_type       = SM_UI8_TYPE;
  newnum->value         = value;
  return newnum;
}

// Return an sm_string describing this ui8
sm_string *sm_ui8_to_string(sm_ui8 *self) {
  return sm_new_fstring_at(sms_heap, "ui8(%u)", self->value);
}

// Adds a c string describing the ui8 to the buffer
// Returns the length
uint32_t sm_ui8_sprint(sm_ui8 *self, char *buffer, bool fake) {
  char internal_buf[10];
  if (fake)
    buffer = internal_buf;
  return sprintf(buffer, "ui8(%u)", self->value);
}
