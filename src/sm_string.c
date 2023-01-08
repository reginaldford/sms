#include "sms.h"

// safe string copy
char *sm_strncpy(char *dest, const char *src, unsigned int n) {
  unsigned int i;
  for (i = 0; i < n && src[i] != '\0'; i++)
    dest[i] = src[i];
  for (; i <= n; i++)
    dest[i] = '\0';
  return dest;
}

// expecting a null terminated string!
sm_string *sm_new_string(unsigned int size, char *str) {
  // We add a null character that is not included in the size
  struct sm_string *newstr = (sm_string *)sm_malloc(sm_round_size(sizeof(sm_string) + size + 1));
  newstr->my_type          = sm_string_type;
  newstr->size             = size;
  sm_strncpy(&(newstr->content), str, size);
  return newstr;
}

sm_string *sm_concat_strings(sm_string *str1, sm_string *str2) {
  unsigned int size_sum   = str1->size + str2->size;
  sm_string   *new_string = sm_new_string(size_sum, &(str1->content));
  sm_strncpy(&(new_string->content) + str1->size, &(str2->content), str2->size);
  return new_string;
}
