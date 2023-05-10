// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"

// Return the index of the first instance of this substring in str
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

// Return an array with all of the parts of str,
// split up into substrings, where the instances of needle are separated
// So, strSplit("abc123abc123","c") == [ "ab" , "c", "123ab", "c" , "123" ]
// The needle in the result is the same instance as the one given in the input.
sm_expr *sm_str_split(sm_string *str, sm_string *needle) {
  if (needle->size == 0 || str->size == 0)
    return sm_new_expr(SM_ARRAY_EXPR, (sm_object *)sm_new_string(str->size, &str->content));
  unsigned int i, j, num_needles = 0;
  unsigned int needles[str->size / needle->size + 1];
  char        *str_cstr    = &(str->content);
  char        *needle_cstr = &(needle->content);
  // find all the occurrences of the needle in the string
  for (i = 0; i <= str->size - needle->size; i++) {
    for (j = 0; j < needle->size; j++) {
      if (str_cstr[i + j] != needle_cstr[j]) {
        break;
      }
    }
    if (j == needle->size) {
      needles[num_needles] = i;
      num_needles++;
    }
  }
  // Shortcut for no needles
  if (num_needles == 0)
    return sm_new_expr(SM_ARRAY_EXPR, (sm_object *)sm_new_string(str->size, &str->content));
  // Count the number of adjacent needles
  unsigned int num_adjacents = 0;
  for (unsigned int i = 0; i + 1 < num_needles; i++) {
    if (needles[i] + needle->size == needles[i + 1])
      num_adjacents++;
  }
  // Totaling the string parts outside of the outtermost needles in the string
  unsigned int border_chunks = 0;
  if (needles[0] != 0)
    border_chunks++;
  if (needles[num_needles - 1] + needle->size != str->size)
    border_chunks++;
  // This should be the exact capacity necessary to store the string parts
  unsigned int capacity = num_needles * 2 - 1 + border_chunks - num_adjacents;
  // Create and return the resulting array expression
  sm_expr     *result         = sm_new_expr_n(SM_ARRAY_EXPR, 0, capacity);
  unsigned int current_needle = 0;
  unsigned int current_result = 0;
  i                           = 0;
  while (i < str->size) {
    if (i == needles[current_needle]) {
      sm_expr_set_arg(result, current_result, (sm_object *)needle);
      current_needle++;
      i += needle->size;
      current_result++;
      continue;
    }
    unsigned int len;
    if (current_needle < num_needles)
      len = needles[current_needle] - i;
    else
      len = str->size - i;
    sm_string *new_segment = sm_new_string(len, &str_cstr[i]);
    sm_expr_set_arg(result, current_result, (sm_object *)new_segment);
    current_result++;
    i += len;
  }
  result->size = current_result;
  return result;
}
