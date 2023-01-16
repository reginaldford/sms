// The following file is provided under the BSD 2-clause license. For more info, read LICENSE.txt.

#include "sms.h"

sm_double *sm_new_double(double value) {
  struct sm_double *newnum = (sm_double *)sm_malloc(sizeof(sm_double));
  newnum->my_type          = sm_double_type;
  newnum->value            = value;
  return newnum;
}

// Return an sm_string describing this double
sm_string *sm_double_to_string(sm_double *self) {
  sm_string *new_str = sm_new_string(sm_double_to_string_len(self), "");
  sm_double_sprint(self, &(new_str->content));
  (&new_str->content)[new_str->size] = '\0';
  return new_str;
}

// Return an sm_string describing this double
// Unfortunately requires expressing the double ahead of time
unsigned int sm_double_to_string_len(sm_double *self) {
  char buffer[17];
  return sm_double_sprint(self, buffer);
}

// Adds a c string describing the double to the buffer
// Returns the length
unsigned int sm_double_sprint(sm_double *self, char *buffer) {
  sprintf(buffer, "%.*g", 16, self->value);
  unsigned short int count = 0;
  for (; count < 18 && buffer[count] != '\0'; count++)
    ;
  return count;
}
