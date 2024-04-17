// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"

extern sm_cx *sms_scratch;

// bison generated parser's global line number :-(
extern int yylineno;

// Completion func
void sm_terminal_completion(const char *buf, linenoiseCompletions *lc) {
  sm_expr *emptyExpr = sm_new_expr_n(SM_ARRAY_EXPR, 0, 0);
  sm_cx   *scratch   = (sm_cx *)(*sm_global_lex_stack(NULL)->top);
  sm_expr *keys      = sm_node_keys(scratch->content, sm_new_stack_obj(17), emptyExpr);
  if (keys->size > 0) {
    sm_symbol *first = (sm_symbol *)sm_expr_get_arg(keys, 0);

    for (int i = 1; i < keys->size; i++) {
      sm_symbol *current_key = (sm_symbol *)sm_expr_get_arg(keys, i);
      if (strncmp(&current_key->name->content, &first->name->content,
                  MIN(first->name->size, current_key->name->size)) < 0) {
        first = current_key;
      }
    }
    linenoiseAddCompletion(lc, &first->name->content);
  }
}

// Print the prompt
sm_string *sm_terminal_prompt() {
  static uint8_t escape_attempts = 0;
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
    sm_string *output = sm_new_string(len, line);
    free(line);
    return output;
  } else {
    escape_attempts++;
    if (escape_attempts == 1)
      printf("Press (Ctrl+d/Ctrl+c) again to exit.\n");
    else {
      printf("User requested exit.\n");
      sm_signal_exit(0);
    }
  }
  return sm_new_string(0, "");
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
