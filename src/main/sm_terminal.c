// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "sms.h"
extern int yylineno;
void       sm_terminal_prompt() {
  printf("\n%i> ", yylineno);
  fflush(stdout);
}
