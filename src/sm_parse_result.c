// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "sms.h"
#include "y.tab.h"

struct sm_parse_result sm_parse() {
  int result = yyparse();
  return (sm_parse_result){.return_val = result, .parsed_object = sm_global_parser_output(NULL)};
}