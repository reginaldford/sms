// This project is licensed under the BSD 2 clause license. See LICENSE.txt for more information.
#include "sms.h"

sm_double *sm_new_double(double value) {
  struct sm_double *newnum = (sm_double *)sm_malloc(sizeof(sm_double));
  newnum->my_type          = sm_double_type;
  newnum->value            = value;
  return newnum;
}

sm_string *sm_double_to_string(sm_double *self) {
  sm_double *n1 = (sm_double *)self;
  char       string_space[20];
  double     d = n1->value;
  sprintf(string_space, "%.*g", (int)d == d ? 17 : 17, d);
  return sm_new_string(strlen(string_space), string_space);
}
