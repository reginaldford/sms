// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

// Store an error
// source is NULL or "file : line : col" or similar
typedef struct sm_error {
  int16_t    my_type;
  sm_symbol *title;
  sm_string *message;
  sm_string *source;
  int32_t    line;
  sm_cx     *notes;
} sm_error;

sm_error *sm_new_error(int title_len, char *title_str, int message_len, char *message_str,
                       int sourceLen, char *source, uint32_t line);
int       sm_err_sprint(sm_error *self, char *buffer, bool fake);
sm_error *sm_new_error_blank();
sm_error *sm_new_error_from_expr(sm_string* title, sm_string* message ,sm_expr * sme, sm_cx* notes) ;
