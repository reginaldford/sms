// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

typedef struct sm_parse_result {
  int        return_val;
  sm_object *parsed_object;
} sm_parse_result;

sm_parse_result sm_parse_stdin();
sm_parse_result sm_parse_more();
sm_parse_result sm_parse_cstr(char *cstr, int len);
sm_parse_result sm_parse_file(char *fname);
void            sm_parse_done();
