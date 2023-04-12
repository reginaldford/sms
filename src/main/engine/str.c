// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"

sm_double *sm_str_find(sm_string *str, sm_string *to_find) {
  const char *cstr        = &(str)->content;
  const char *to_findcstr = &(to_find)->content;
  const char *result      = strstr(cstr, to_findcstr);
  if (result != NULL) {
    return sm_new_double(result - cstr);
  } else {
    return sm_new_double(-1);
  }
}
