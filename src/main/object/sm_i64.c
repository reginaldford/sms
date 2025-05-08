// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"

// Return new object encapsulating 64 bit signed integer value
sm_i64 *sm_new_i64(int64_t value) {
  struct sm_i64 *newnum = (sm_i64 *)sm_malloc(sizeof(sm_i64));
  newnum->my_type       = SM_I64_TYPE;
  newnum->value         = value;
  return newnum;
}

// Return new object encapsulating 64 bit signed integer value
sm_i64 *sm_new_i64_at(sm_heap *h, int64_t value) {
  struct sm_i64 *newnum = (sm_i64 *)sm_malloc_at(h, sizeof(sm_i64));
  newnum->my_type       = SM_I64_TYPE;
  newnum->value         = value;
  return newnum;
}

// Return an sm_string describing this I64
sm_string *sm_i64_to_string(sm_i64 *self) {
  sm_string *new_str = sm_new_string(sm_i64_sprint(self, NULL, true), "");
  sm_i64_sprint(self, &(new_str->content), false);
  (&new_str->content)[new_str->size] = '\0';
  return new_str;
}

// Adds a c string describing the i64 to the buffer
// Returns the length
uint32_t sm_i64_sprint(sm_i64 *self, char *buffer, bool fake) {
  char internal_buf[30];
  if (!fake)
    snprintf(buffer, 30, "i64(%lli)", self->value);
  else {
    snprintf(internal_buf, 30, "i64(%lli)", self->value);
    buffer = internal_buf;
  }
  uint16_t count = 0;
  while (count < 30 && buffer[count] != '\0')
    count++;
  return count;
}
