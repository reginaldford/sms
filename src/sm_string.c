// The following file is provided under the BSD 2-clause license. For more info, read LICENSE.txt.

#include "sms.h"

// Safe string copy
char *sm_strncpy(char *dest, const char *src, unsigned int n) {
  unsigned int i;
  for (i = 0; i < n && src[i] != '\0'; i++)
    dest[i] = src[i];
  for (; i <= n; i++)
    dest[i] = '\0';
  return dest;
}

// Expecting a null terminated string
sm_string *sm_new_string(unsigned int size, char *str) {
  // We add a null character that is not included in the size
  struct sm_string *newstr = (sm_string *)sm_malloc(sm_round_size(sizeof(sm_string) + size + 1));
  newstr->my_type          = sm_string_type;
  newstr->size             = size;
  sm_strncpy(&(newstr->content), str, size);
  return newstr;
}

// Return a new string: str1+str2
sm_string *sm_string_add(sm_string *str1, sm_string *str2) {
  unsigned int size_sum   = str1->size + str2->size;
  sm_string   *new_string = sm_new_string(size_sum, &(str1->content));
  sm_strncpy(&(new_string->content) + str1->size, &(str2->content), str2->size);
  return new_string;
}

// Return a new string: str1+str2
// Replace the previous strings with new spaces
sm_string *sm_string_add_recycle(sm_string *str1, sm_string *str2) {
  sm_string *new_string = sm_string_add(str1, str2);
  sm_new_space(str1, sm_sizeof((sm_object *)str1));
  sm_new_space(str2, sm_sizeof((sm_object *)str2));
  return new_string;
}

// Recycle only str1
sm_string *sm_string_add_recycle_1st(sm_string *str1, sm_string *str2) {
  sm_string *new_string = sm_string_add(str1, str2);
  sm_new_space(str1, sm_sizeof((sm_object *)str1));
  return new_string;
}

// Recycle only str2
sm_string *sm_string_add_recycle_2nd(sm_string *str1, sm_string *str2) {
  sm_string *new_string = sm_string_add(str1, str2);
  sm_new_space(str2, sm_sizeof((sm_object *)str2));
  return new_string;
}

// Adds quotes, unescapes, produces a new sm_string
sm_string *sm_string_to_string(sm_string *str) {
  const unsigned int final_len = str->size + 2;
  sm_string         *new_str   = sm_new_string(final_len, "");
  sm_string_sprint(str, &(new_str->content));
  (&new_str->content)[final_len] = '\0';
  return new_str;
}

// Adds a c string describing the string
// Unescapes back to the c-style escape codes
// Returns the length
unsigned int sm_string_sprint(sm_string *self, char *to_str) {
  char *from_str = &(self->content);
  to_str[0]      = '\"';
  // i for from_str , j for to_str
  unsigned int i, j;
  for (i = 0, j = 1; i < self->size; i++, j++) {
    if (from_str[i] == '\n' || from_str[i] == '\r' || from_str[i] == '\t' || from_str[i] == '\\' ||
        from_str[i] == '\"') {
      to_str[j++] = '\\';
      switch (from_str[i]) {
      case '\n':
        to_str[j] = 'n';
        break;
      case '\r':
        to_str[j] = 'r';
        break;
      case '\t':
        to_str[j] = 't';
        break;
      case '\\':
        to_str[j] = '\\';
        break;
      case '\"':
        to_str[j] = '\"';
        break;
      default:
        to_str[j] = from_str[i];
      }
    } else {
      to_str[j] = from_str[i];
    }
  }
  to_str[j] = '\"';
  return j + 1;
}
