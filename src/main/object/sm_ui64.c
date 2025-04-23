// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"

// Return new object encapsulating 64 bit unsigned integer value
sm_ui64 *sm_new_ui64(ui64 value) {
  struct sm_ui64 *newnum = (sm_ui64 *)sm_malloc(sizeof(sm_ui64));
  newnum->my_type        = SM_UI64_TYPE;
  newnum->value          = value;
  return newnum;
}

// Return new object encapsulating 64 bit unsigned integer value
sm_ui64 *sm_new_ui64_at(sm_heap *h, ui64 value) {
  struct sm_ui64 *newnum = (sm_ui64 *)sm_malloc_at(h, sizeof(sm_ui64));
  newnum->my_type        = SM_UI64_TYPE;
  newnum->value          = value;
  return newnum;
}

// Return an sm_string describing this ui64
sm_string *sm_ui64_to_string(sm_ui64 *self) {
  sm_string *new_str = sm_new_string(sm_ui64_sprint(self, NULL, true), "");
  sm_ui64_sprint(self, &(new_str->content), false);
  (&new_str->content)[new_str->size] = '\0';
  return new_str;
}

// Adds a c string describing the ui64 to the buffer
// Returns the length
uint32_t sm_ui64_sprint(sm_ui64 *self, char *buffer, bool fake) {
  char internal_buf[30];
  if (!fake)
    snprintf(buffer, 30, "ui64(%llu)", self->value);
  else {
    snprintf(internal_buf, 30, "ui64(%llu)", self->value);
    buffer = internal_buf;
  }
  uint16_t count = 0;
  while (count < 30 && buffer[count] != '\0')
    count++;
  return count;
}
