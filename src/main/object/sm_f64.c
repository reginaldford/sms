// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"

sm_f64 *sm_new_f64(f64 value) {
  struct sm_f64 *newnum = (sm_f64 *)sm_malloc(sizeof(sm_f64));
  newnum->my_type       = SM_F64_TYPE;
  newnum->value         = value;
  return newnum;
}

// Return an sm_string describing this f64
sm_string *sm_f64_to_string(sm_f64 *self) {
  sm_string *new_str = sm_new_string(sm_f64_sprint(self, NULL, true), "");
  sm_f64_sprint(self, &(new_str->content), false);
  (&new_str->content)[new_str->size] = '\0';
  return new_str;
}

// Adds a c string describing the f64 to the buffer
// Returns the length
uint32_t sm_f64_sprint(sm_f64 *self, char *buffer, bool fake) {
  char internal_buf[24];
  if (!fake)
    snprintf(buffer, 23, "%.16g", self->value);
  else {
    snprintf(internal_buf, 23, "%.16g", self->value);
    buffer = internal_buf;
  }
  uint16_t count = 0;
  for (; count < 24 && buffer[count] != '\0'; count++)
    ;
  return count;
}
