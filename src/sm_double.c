#include "sms.h"

sm_double *sm_new_double(double value)
{
  struct sm_double *newnum = (sm_double*)sm_malloc(sizeof(sm_double));
  newnum->my_type = sm_double_type;
  newnum->value = value;
  return newnum;
}
