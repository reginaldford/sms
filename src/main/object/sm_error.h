// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

// Store an error
// The source is either /dev/stdin or
// the filepath of the file being parsed.
typedef struct sm_error {
  int16_t    my_type;
  sm_string *message;
  sm_string *source;
  uint32_t   line;
} sm_error;

sm_error *sm_new_error(sm_string *message, sm_string *source, uint32_t line);
int       sm_error_sprint(sm_error *self, char *buffer, bool fake);