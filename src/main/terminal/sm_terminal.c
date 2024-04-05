// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"

// bison generated parser's global line number :-(
extern int yylineno;

// Print the prompt
sm_string *sm_terminal_prompt() {
  static int escape_attempts = 0;
  linenoiseSetMultiLine(1);
  char prompt[17];
  int  cursor = 0;
  if (yylineno > 1) {
    printf("\n");
    fflush(stdout);
  }
  cursor += sprintf(prompt + cursor, "%i> ", yylineno);
  sm_env *env = sm_global_environment(NULL);
  if (!env->no_history_file)
    linenoiseHistoryLoad(env->history_file);
  char *line = linenoise(prompt);
  if (line) {
    escape_attempts = 0;
    int len         = strlen(line);
    linenoiseHistoryAdd(line);
    char historyFilePath;
    if (!env->no_history_file)
      linenoiseHistorySave(env->history_file);
    return sm_new_string(len, line);
  } else {
    escape_attempts++;
    printf("Repeat (ctrl+d/ctrl+c) to exit.\n");
    if (escape_attempts >= 2)
      sm_signal_exit(0);
    return sm_new_string(0, "");
  }
}

bool sm_terminal_has_color() {
  // Only call getenv once
  static char *term = NULL;
  if (!term)
    term = getenv("TERM");
  // If the word color is in there, we will assume
  // that the ANSI 16 color escapes are available
  if (term != NULL && strstr(term, "color") != NULL)
    return true;
  // Terminal does not support 16 colors
  return false;
}

// v100 code to set the foreground color
char *sm_terminal_fg_color(sm_terminal_color color) {
  // We must provide this sequence so that printf adds nothing for color commands
  // in a non-color terminal
  static char *codes[] = {
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
    return "";

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
  if (sm_terminal_has_color() && color >= SM_TERM_BLACK && color <= SM_TERM_B_WHITE)
    return codes[color];
  return "";
}

// v100 code to reset
char *sm_terminal_reset() {
  static char *reset_code = "\x1b[0m";
  if (sm_terminal_has_color())
    return reset_code;
  else
    return "";
}
