// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

// Null termination character excluded in size field.
// Zero size string still has NULL termination.
typedef struct sm_string {
  short int    my_type;
  unsigned int size;
  int          space; // make string at least 16 bytes
  char         content;
} sm_string;

char        *sm_strncpy(char *dest, const char *src, unsigned int n);
sm_string   *sm_new_string(unsigned int size, char *str);
sm_string   *sm_string_add(sm_string *str1, sm_string *str2);
sm_string   *sm_string_add_recycle(sm_string *str1, sm_string *str2);
sm_string   *sm_new_string_of(unsigned int size, sm_string *str);
sm_string   *sm_string_add_recycle_1st(sm_string *str1, sm_string *str2);
sm_string   *sm_string_add_recycle_2nd(sm_string *str1, sm_string *str2);
sm_string   *sm_string_to_string(sm_string *str);
unsigned int sm_string_sprint(sm_string *self, char *buffer, bool fake);
sm_string   *sm_new_string_manual(unsigned int size);
sm_string   *sm_string_escape(sm_string *input);
