// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

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
  struct sm_string *newstr = sm_new_string_manual(size);
  sm_strncpy(&(newstr->content), str, size);
  return newstr;
}

// For you to fill in yourself
sm_string *sm_new_string_manual(unsigned int size) {
  struct sm_string *newstr = (sm_string *)sm_malloc(sm_round_size(sizeof(sm_string) + size));
  newstr->my_type          = SM_STRING_TYPE;
  newstr->size             = size;
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
  sm_string_sprint(str, &(new_str->content), false);
  (&new_str->content)[final_len] = '\0';
  return new_str;
}

// Adds a c string describing the string
// Unescapes back to the c-style escape codes
// Returns the length
unsigned int sm_string_sprint(sm_string *self, char *to_str, bool fake) {
  char *from_str = &(self->content);
  if (!fake)
    to_str[0] = '\"';
  // i for from_str , j for to_str
  unsigned int i, j;
  for (i = 0, j = 1; i < self->size; i++, j++) {
    if (from_str[i] == '\n' || from_str[i] == '\r' || from_str[i] == '\t' || from_str[i] == '\\' ||
        from_str[i] == '\"') {
      if (!fake)
        to_str[j] = '\\';
      j++;
      switch (from_str[i]) {
      case '\n':
        if (!fake)
          to_str[j] = 'n';
        break;
      case '\r':
        if (!fake)
          to_str[j] = 'r';
        break;
      case '\t':
        if (!fake)
          to_str[j] = 't';
        break;
      case '\\':
        if (!fake)
          to_str[j] = '\\';
        break;
      case '\"':
        if (!fake)
          to_str[j] = '\"';
        break;
      default:
        if (!fake)
          to_str[j] = from_str[i];
      }
    } else {
      if (!fake)
        to_str[j] = from_str[i];
    }
  }
  if (!fake)
    to_str[j] = '\"';
  return j + 1;
}


// Escapes a cstr to another cstr with c style escapes
// Returns the length
sm_string *sm_string_escape(sm_string *input) {
  unsigned int len = input->size;
  // Replace the escape codes in place
  unsigned int i        = 0; // Index during the scan of input string
  int          final_i  = 0; // Index of output
  char        *from_str = &input->content;
  char         to_str[input->size];
  while (i < len) {
    if (from_str[i] == '\\') {
      // This is an escape code. Replace it with the appropriate character.
      switch (from_str[i + 1]) {
      case 'n':
        to_str[final_i] = '\n';
        break;
      case 't':
        to_str[final_i] = '\t';
        break;
      case 'r':
        to_str[final_i] = '\r';
        break;
      case '\\':
        to_str[final_i] = '\\';
        break;
      case '"':
        to_str[final_i] = '\"';
        break;
      default: // Unrecognized escape. backslash as default
        to_str[final_i] = '\\';
        break;
      }
      i += 2; // Skip the escape code
    } else {
      // Pass through regular characters
      to_str[final_i] = from_str[i];
      i++;
    }
    final_i++;
  }
  return sm_new_string(final_i, to_str);
}
