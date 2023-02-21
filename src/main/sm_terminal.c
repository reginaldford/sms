// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "sms.h"

void sm_terminal_prompt() {
  if (!sm_global_options(NULL)->script_flag) {
    printf("\n> ");
    fflush(stdout);
  }
}
