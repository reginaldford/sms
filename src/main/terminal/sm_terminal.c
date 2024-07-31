// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"

extern sm_heap *sms_heap;
// bison generated parser's global line number :-(
extern int yylineno;

// Adds the keys of cx to linenoise completion set
void add_keys(sm_cx *cx, sm_expr *keys, const char *buf, int buf_len, const char *last_sym,
              linenoiseCompletions *lc) {
  char concat_space[512];
  int  last_sym_len = 0;
  if (last_sym)
    last_sym_len = strlen(last_sym);
  sm_expr *empty_expr = sm_new_expr_n(SM_TUPLE_EXPR, 0, 8, NULL);
  keys                = sm_node_keys(cx->content, sm_new_stack_obj(17), empty_expr);
  for (int i = 0; i < keys->size; i++) {
    sm_symbol *sym = (sm_symbol *)sm_expr_get_arg(keys, i);
    if (!strncmp(last_sym, &sym->name->content, MIN(last_sym_len, sym->name->size))) {
      int completion_len = (sym->name->size) + buf_len;
      sm_strncpy(concat_space + buf_len - last_sym_len, &sym->name->content, sym->name->size);
      concat_space[buf_len + sym->name->size] = '\0';
      linenoiseAddCompletion(lc, concat_space);
    }
  }
}

// Completion func. This is the interface to linenoise.
// This is called when the tab key is pressed.
void sm_terminal_completion(const char *buf, linenoiseCompletions *lc) {
  sm_expr    *empty_expr = sm_new_expr_n(SM_TUPLE_EXPR, 0, 0, NULL);
  sm_cx      *scratch    = (sm_cx *)(*sm_global_lex_stack(NULL)->top);
  sm_expr    *keys       = sm_node_keys(scratch->content, sm_new_stack_obj(17), empty_expr);
  const char *last_sym;
  int         len          = strlen(buf);
  int         last_sym_pos = len;
  // Determine the last_sym_pos, the beginning of the last symbol
  while (last_sym_pos >= 0) {
    if (!sm_is_symbol_char(buf[last_sym_pos - 1]))
      break;
    last_sym_pos--;
  }
  // We signify no last sym by setting last_sym to NULL
  if (last_sym_pos < 0)
    last_sym = NULL;
  else
    last_sym = buf + last_sym_pos;
  add_keys(scratch, keys, buf, len, last_sym, lc);
  if (scratch->parent)
    add_keys(scratch->parent, keys, buf, len, last_sym, lc);
}

// Print the prompt, return an sm_parse_result
sm_parse_result sm_terminal_prompt(bool plain_mode) {
  if (plain_mode)
    return sm_terminal_prompt_plain();
  return sm_terminal_prompt_linenoise();
}

// Prompts the user and returns the parsed result.
// Uses plain posix fgets on stdin
sm_parse_result sm_terminal_prompt_plain() {
  printf("\n%i> ", yylineno);
  char  buffer[501];
  char *fgets_val = fgets(buffer, sizeof(buffer), stdin);
  // Handle EOF case
  if (!fgets_val)
    sm_signal_handler(SIGINT);
  int len = strlen(buffer);
  // Automatically adding a semicolon to user's input
  buffer[len]     = ';';
  buffer[len + 1] = '\0';
  return sm_parse_cstr(buffer, len + 1);
}

// Promtps the user and returns the parsed result.
// Users linenoise to provide tab completion
sm_parse_result sm_terminal_prompt_linenoise() {
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
    int        outputLen = len + 1;
    sm_string *output    = sm_new_string(outputLen, line);
    free(line);
    // Automatically add a semicolon !
    (&output->content)[outputLen - 1] = ';';
    return sm_parse_cstr(&output->content, outputLen);
  } else {
    escape_attempts++;
    if (escape_attempts == 1)
      printf("Press (Ctrl+d/Ctrl+c) again to exit.\n");
    else {
      sm_signal_handler(SIGINT);
    }
  }
  // nothing was parsed
  return (sm_parse_result){.return_val = -1, .parsed_object = (sm_object *)sm_new_double(0)};
}

bool sm_terminal_has_color() {
  // Only call getenv once
  static char *term = NULL;
  if (!term)
    term = getenv("TERM");
  // If the word 'color' is in the TERM environment variable, we will assume
  // that the ANSI 16 color escapes are available
  if (term != NULL && strstr(term, "color") != NULL)
    return true;
  // Terminal does not support 16 colors
  return false;
}

// vt100 code to set the foreground color
// Provides NULL for bw terminals
char *sm_terminal_fg_color(sm_terminal_color color) {
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

// vt100 code to set the background color
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
  if (!sm_terminal_has_color())
    return "";
  if (color >= SM_TERM_BLACK && color <= SM_TERM_B_WHITE)
    return codes[color];
  return "";
}

// vt100 code to reset
char *sm_terminal_reset() {
  static char *reset_code = "\x1b[0m";
  if (sm_terminal_has_color())
    return reset_code;
  else
    return "";
}
