// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

// 16 color system
typedef enum sm_terminal_color {
  SM_TERM_BLACK,
  SM_TERM_RED,
  SM_TERM_GREEN,
  SM_TERM_YELLOW,
  SM_TERM_BLUE,
  SM_TERM_MAGENTA,
  SM_TERM_CYAN,
  SM_TERM_WHITE,
  SM_TERM_B_BLACK,
  SM_TERM_B_RED,
  SM_TERM_B_GREEN,
  SM_TERM_B_YELLOW,
  SM_TERM_B_BLUE,
  SM_TERM_B_MAGENTA,
  SM_TERM_B_CYAN,
  SM_TERM_B_WHITE
} sm_terminal_color;

sm_string *sm_terminal_prompt();
bool       sm_terminal_has_color();
char      *sm_terminal_fg_color(sm_terminal_color color);
char      *sm_terminal_bg_color(sm_terminal_color color);
char      *sm_terminal_reset();
void       sm_terminal_completion(const char *buf, linenoiseCompletions *lc);
