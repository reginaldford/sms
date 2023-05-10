// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "sms.h"
extern int yylineno;
void       sm_terminal_prompt() {
  if (yylineno == 1)
    printf("\n");
  printf("%i> ", yylineno);
  fflush(stdout);
}
