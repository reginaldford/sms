// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"
#include "../../bison_flex/y.tab.h"

void  scan_str(const char *cstr, int len);
void  end_scan_str(void);
FILE *lex_file(char *filepath);
void  lex_cstr(char *cstr, int len);
void  _done_lexing_file(FILE *f);
int   yyparse(void);

// Parse a string with known length
sm_parse_result sm_parse_cstr(char *cstr, int len) {
  lex_cstr(cstr, len);
  int result = yyparse();
  return (sm_parse_result){.return_val = result, .parsed_object = sm_global_parser_output(NULL)};
}

// Run this the first time to begin parsing a file
sm_parse_result sm_parse_file(char *fname) {
  FILE *success = lex_file(fname);
  if (!success) {
    printf("Error: There was a problem with opening the file: %s\n", fname);
    return (sm_parse_result){.return_val = -1, .parsed_object = (sm_object *)sm_new_f64(0)};
  }
  int result = yyparse();
  _done_lexing_file(success);
  return (sm_parse_result){.return_val = result, .parsed_object = sm_global_parser_output(NULL)};
}
