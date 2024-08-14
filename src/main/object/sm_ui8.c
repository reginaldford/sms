// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"

sm_ui8 *sm_new_ui8(ui8 value) {
  struct sm_ui8 *newnum = (sm_ui8 *)sm_malloc(sizeof(sm_ui8));
  newnum->my_type       = SM_UI8_TYPE;
  newnum->value         = value;
  return newnum;
}

// Return an sm_string describing this ui8
sm_string *sm_ui8_to_string(sm_ui8 *self) {
  sm_string *new_str = sm_new_string(sm_ui8_sprint(self, NULL, true), "");
  sm_ui8_sprint(self, &(new_str->content), false);
  (&new_str->content)[new_str->size] = '\0';
  return new_str;
}

// Adds a c string describing the ui8 to the buffer
// Returns the length
uint32_t sm_ui8_sprint(sm_ui8 *self, char *buffer, bool fake) {
  char internal_buf[10];
  if (!fake)
    snprintf(buffer, 10, "ui8(%i)", self->value);
  else {
    snprintf(internal_buf, 10, "ui8(%i)", self->value);
    buffer = internal_buf;
  }
  uint16_t count = 0;
  while (count < 10 && buffer[count] != '\0')
    count++;
  return count;
}
