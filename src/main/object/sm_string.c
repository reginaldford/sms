// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"

extern sm_heap *sms_heap;


// string copy with known length. Does not add NULL termination
char *sm_strncpy(char *dest, const char *src, uint32_t n) {
  uint32_t i;
  for (i = 0; i < n; i++)
    dest[i] = src[i];
  dest[i] = '\0';
  return dest;
}

// string copy with known length. Does not add NULL termination
char *sm_strncpy_unsafe(char *dest, const char *src, uint32_t n) {
  uint32_t i;
  for (i = 0; i < n; i++)
    dest[i] = src[i];
  return dest;
}

// Create a new string in specified heap, automatically null terminated
sm_string *sm_new_string(uint32_t size, char *str) {
  struct sm_string *newstr = sm_new_string_manual(size);
  sm_strncpy(&newstr->content, str, size);
  return newstr;
}

// Create a new string in specified heap, automatically null terminated
sm_string *sm_new_string_at(struct sm_heap *heap, uint32_t size, char *str) {
  struct sm_string *newstr = sm_new_string_manual_at(heap, size);
  sm_strncpy(&newstr->content, str, size);
  return newstr;
}
// Make a string with a format spec, like printf, using variable arguments
struct sm_string *sm_new_fstring_at(struct sm_heap *heap, const char *format, ...) {
  va_list args;
  va_start(args, format);

  // Create a copy of args for the second vsnprintf call
  va_list args_copy;
  va_copy(args_copy, args);

  int length = vsnprintf(NULL, 0, format, args);
  if (length < 0) {
    va_end(args);
    va_end(args_copy);
    return NULL;
  }

  struct sm_string *newstr = sm_new_string_manual_at(heap, length + 1);
  if (newstr == NULL) {
    va_end(args);
    va_end(args_copy);
    return NULL;
  }

  vsnprintf(&newstr->content, length + 1, format, args_copy);
  va_end(args);
  va_end(args_copy);

  newstr->size = length;
  return newstr;
}

// Return a new empty string (does not nullify contents)
sm_string *sm_new_string_manual_at(struct sm_heap *heap, uint32_t size) {
  struct sm_string *newstr =
    (sm_string *)sm_malloc_at(heap, sizeof(sm_string) + sm_round_size64(size));
  newstr->my_type = SM_STRING_TYPE;
  newstr->size    = size;
  return newstr;
}

// Return a new empty string (does not nullify contents)
sm_string *sm_new_string_manual(uint32_t size) { return sm_new_string_manual_at(sms_heap, size); }

// Return a new string: str1+str2
sm_string *sm_string_add(sm_string *str1, sm_string *str2) {
  uint32_t   size_sum   = str1->size + str2->size;
  sm_string *new_string = sm_new_string(size_sum, &(str1->content));
  sm_strncpy(&new_string->content + str1->size, &(str2->content), str2->size);
  return new_string;
}

// Adds quotes, unescapes, produces a new sm_string
sm_string *sm_string_to_string(sm_string *str) {
  const uint32_t final_len = str->size + 2;
  sm_string     *new_str   = sm_new_string(final_len, "");
  const uint32_t len       = sm_string_sprint(str, &new_str->content, false);
  (&new_str->content)[len] = '\0';
  new_str->size            = len;
  return new_str;
}

// Adds a c string describing the string
// Unescapes back to the c-style escape codes
// Returns the length
uint32_t sm_string_sprint(sm_string *self, char *to_str, bool fake) {
  char *from_str = &self->content;
  if (!fake)
    to_str[0] = '\"';
  // i for from_str , j for to_str
  uint32_t i, j;
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
  uint32_t len = input->size;
  // Replace the escape codes in place
  uint32_t i        = 0; // Index during the scan of input string
  int      final_i  = 0; // Index of output
  char    *from_str = &input->content;
  char     to_str[input->size];
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

// Returns whether two strings match
bool sm_string_is_equal(sm_string *str1, sm_string *str2) {
  if (str1->size != str2->size)
    return false;
  for (uint32_t i = 0; i < str1->size; ++i)
    if ((&str1->content)[i] != (&str2->content)[i])
      return false;
  return true;
}
