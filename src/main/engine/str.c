// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"

// Return the index of the first instance of this substring in str or false
sm_object *sm_str_find(sm_string *str, sm_string *to_find) {
  const char *cstr        = &(str)->content;
  const char *to_findcstr = &(to_find)->content;
  const char *result      = strstr(cstr, to_findcstr);
  if (result != NULL) {
    return (sm_object *)sm_new_f64(result - cstr);
  } else {
    return (sm_object *)sms_false;
  }
}

// Function to find substring in reverse
sm_object *sm_str_findr(sm_string *haystack, sm_string *needle) {
  uint32_t haystack_size = haystack->size;
  uint32_t needle_size   = needle->size;
  if (needle_size > haystack_size)
    return (sm_object *)sms_false; // Needle is longer than haystack
  // Iterate from the end of the haystack
  for (int32_t i = haystack_size - needle_size; i >= 0; --i)
    if (!strncmp(&haystack->content + i, &needle->content, needle_size))
      return (sm_object *)sm_new_f64(i);
  return (sm_object *)sms_false; // Needle not found
}
// Return a tuple with all of the parts of str,
// split up into substrings, where the instances of needle are separated
// So, strSplit("abc123abc123","c") == [ "ab" , "c", "123ab", "c" , "123" ]
// The needle in the result is the same instance as the one given in the input.
sm_expr *sm_str_split(sm_string *str, sm_string *needle) {
  if (needle->size == 0 || str->size == 0)
    return sm_new_expr(SM_TUPLE_EXPR, (sm_object *)sm_new_string(str->size, &str->content), NULL);
  uint32_t i, j, num_needles = 0;
  uint32_t needles[str->size / needle->size + 1];
  char    *str_cstr    = &(str->content);
  char    *needle_cstr = &(needle->content);
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
    return sm_new_expr(SM_TUPLE_EXPR, (sm_object *)sm_new_string(str->size, &str->content), NULL);
  // Count the number of adjacent needles
  uint32_t num_adjacents = 0;
  for (uint32_t i = 0; i + 1 < num_needles; i++) {
    if (needles[i] + needle->size == needles[i + 1])
      num_adjacents++;
  }
  // Totaling the string parts outside of the outtermost needles in the string
  uint32_t border_chunks = 0;
  if (needles[0] != 0)
    border_chunks++;
  if (needles[num_needles - 1] + needle->size != str->size)
    border_chunks++;
  // This should be the exact capacity necessary to store the string parts
  uint32_t capacity = num_needles * 2 - 1 + border_chunks - num_adjacents;
  // Create and return the resulting tuple expression
  sm_expr *result         = sm_new_expr_n(SM_TUPLE_EXPR, 0, capacity, NULL);
  uint32_t current_needle = 0;
  uint32_t current_result = 0;
  i                       = 0;
  while (i < str->size) {
    if (i == needles[current_needle]) {
      sm_expr_set_arg(result, current_result, (sm_object *)needle);
      current_needle++;
      i += needle->size;
      current_result++;
      continue;
    }
    uint32_t len;
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
