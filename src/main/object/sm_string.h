// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

/** @brief  String object structure
 @note Null termination character excluded in size field.
 @note Zero size string still has NULL termination. */
typedef struct sm_string {
  int16_t  my_type;
  uint32_t size;
  int      space;   // make string at least 16 bytes
  char     content; // stores NULL if nothing else
} sm_string;

/// Copy a string of known length
char *sm_strncpy(char *dest, const char *src, uint32_t n);
/// Construct a new sms string from a c string of known length
sm_string *sm_new_string(uint32_t size, char *str);
/// Construct a new sms string in given heap from a c string of known length
sm_string *sm_new_string_at(struct sm_heap *heap, uint32_t size, char *str);
/// Make a string with a format spec, like printf, using variable arguments
struct sm_string *sm_new_fstring_at(struct sm_heap *heap, const char *format, ...);
/// Construct a new sms string in given heap from a c string of known length
sm_string *sm_new_string_manual_at(struct sm_heap *heap, uint32_t size);
/// Create a new string that is the concatenation of str1 and str2
sm_string *sm_string_add(sm_string *str1, sm_string *str2);
/// Adds quotes around the string and puts escape codes for escaped characters
sm_string *sm_string_to_string(sm_string *str);
/// Print to string buffer (unescapes)
uint32_t sm_string_sprint(sm_string *self, char *buffer, bool fake);
/// Make a new empty string to manually fill in
sm_string *sm_new_string_manual(uint32_t size);
/// Performs c-style escaping on the string
sm_string *sm_string_escape(sm_string *input);
