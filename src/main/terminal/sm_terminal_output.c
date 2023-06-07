// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"

// bison generated parser's global line number :-(
extern int yylineno;

// Print the prompt
void sm_terminal_print_prompt() {
  putc('\n', stdout);
  printf("%s", sm_terminal_fg_color(SM_TERM_B_GREEN));
  printf("%i", yylineno);
  putc('>', stdout);
  printf("%s", sm_terminal_reset());
  putc(' ', stdout);
  fflush(stdout);
}


bool sm_terminal_has_color() {
  char *term = getenv("TERM");
  // If the word color is in there, we will assume
  // that the ANSI 16 colors are available
  if (term != NULL && strstr(term, "color") != NULL)
    return true;
  // Terminal does not support 16 colors
  return false;
}

// v100 code to set the foreground color
char *sm_terminal_fg_color(sm_terminal_color color) {
  // We must provide this sequence so that printf adds nothing for color commands
  // in a non-color terminal
  static char *nothing_cstr = " \b";
  static char *codes[]      = {
         "\x1b[30m",   // SM_TERM_BLACK
         "\x1b[31m",   // SM_TERM_RED
         "\x1b[32m",   // SM_TERM_GREEN
         "\x1b[33m",   // SM_TERM_YELLOW
         "\x1b[34m",   // SM_TERM_BLUE
         "\x1b[35m",   // SM_TERM_MAGENTA
         "\x1b[36m",   // SM_TERM_CYAN
         "\x1b[37m",   // SM_TERM_WHITE
         "\x1b[30;1m", // SM_TERM_B_BLACK
         "\x1b[31;1m", // SM_TERM_B_RED
         "\x1b[32;1m", // SM_TERM_B_GREEN
         "\x1b[33;1m", // SM_TERM_B_YELLOW
         "\x1b[34;1m", // SM_TERM_B_BLUE
         "\x1b[35;1m", // SM_TERM_B_MAGENTA
         "\x1b[36;1m", // SM_TERM_B_CYAN
         "\x1b[37;1m"  // SM_TERM_B_WHITE
  };

  if (!sm_terminal_has_color())
    return nothing_cstr;

  if (color >= SM_TERM_BLACK && color <= SM_TERM_B_WHITE)
    return codes[color];
  return "";
}

// v100 code to set the background color
char *sm_terminal_bg_color(sm_terminal_color color) {
  static char *codes[] = {
    "\x1b[40m",   // SM_TERM_BLACK
    "\x1b[41m",   // SM_TERM_RED
    "\x1b[42m",   // SM_TERM_GREEN
    "\x1b[43m",   // SM_TERM_YELLOW
    "\x1b[44m",   // SM_TERM_BLUE
    "\x1b[45m",   // SM_TERM_MAGENTA
    "\x1b[46m",   // SM_TERM_CYAN
    "\x1b[47m",   // SM_TERM_WHITE
    "\x1b[40;1m", // SM_TERM_B_BLACK
    "\x1b[41;1m", // SM_TERM_B_RED
    "\x1b[42;1m", // SM_TERM_B_GREEN
    "\x1b[43;1m", // SM_TERM_B_YELLOW
    "\x1b[44;1m", // SM_TERM_B_BLUE
    "\x1b[45;1m", // SM_TERM_B_MAGENTA
    "\x1b[46;1m", // SM_TERM_B_CYAN
    "\x1b[47;1m"  // SM_TERM_B_WHITE
  };
  if (color >= SM_TERM_BLACK && color <= SM_TERM_B_WHITE)
    return codes[color];
  return "";
}

// v100 code to reset
char *sm_terminal_reset() {
  static char *reset_code = "\x1b[0m";
  return reset_code;
}
