// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

/** @brief  String object structure
 @note Null termination character excluded in size field.
 @note Zero size string still has NULL termination. */
typedef struct sm_string {
  short int    my_type;
  unsigned int size;
  int          space;   // make string at least 16 bytes
  char         content; // stores NULL if nothing else
} sm_string;

/// Copy a string of known length
char *sm_strncpy(char *dest, const char *src, unsigned int n);
/// Construct a new sms string from a c string of known length
sm_string *sm_new_string(unsigned int size, char *str);
/// Create a new string that is the concatenation of str1 and str2
sm_string *sm_string_add(sm_string *str1, sm_string *str2);
/// Destroys the original 2 strings after concatenation
sm_string *sm_string_add_recycle(sm_string *str1, sm_string *str2);
/// Destroys str1 after concatenation
sm_string *sm_string_add_recycle_1st(sm_string *str1, sm_string *str2);
/// Destroys str2 after concatenation
sm_string *sm_string_add_recycle_2nd(sm_string *str1, sm_string *str2);
/// Adds quotes around the string and puts escape codes for escaped characters
sm_string *sm_string_to_string(sm_string *str);
/// Print to string buffer (unescapes)
unsigned int sm_string_sprint(sm_string *self, char *buffer, bool fake);
/// Make a new empty string to manually fill in
sm_string *sm_new_string_manual(unsigned int size);
/// Performs c-style escaping on the string
sm_string *sm_string_escape(sm_string *input);
/// Performs c-style escaping on the string
