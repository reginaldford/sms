// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "sms.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

// yylineno is a global from the bison-generated parser
// storing the current line number
extern int yylineno;

typedef enum { normal, escaped, extended1, extended2 } sm_terminal_state;

int               cursor_pos    = 0;
int               input_len     = 0;
sm_terminal_state current_state = normal;
char              input_buffer[512];
char              escape_buffer[16];
int               eb_cursor = 0;

void process_character(char c) {
  switch (current_state) {
  case normal:
    if (c == 27) { // escape code
      current_state = escaped;
      eb_cursor     = 0;
    } else if (c != 127 && c != 8) { // not backspace
      putc(c, stdout);
      fflush(stdout);
      input_buffer[input_len++] = c;
    } else if (input_len > 0) { // backspace
      putc('\b', stdout);
      putc(' ', stdout);
      putc('\b', stdout);
      fflush(stdout);
      input_buffer[input_len] = '\0';
      input_len--;
    }
    break;
  case extended1: // c is last char or this is an extended escape
    escape_buffer[eb_cursor++] = c;
    if (c == 49) {
      current_state = extended2;
    } else {
      escape_buffer[eb_cursor++] = '\0';
      current_state              = normal;
      eb_cursor                  = 0;
    }
    break;
  case extended2:
    if (eb_cursor >= 4) { // extended sequence is 5 total characters
      escape_buffer[eb_cursor++] = c;
      escape_buffer[eb_cursor++] = '\0';
      eb_cursor                  = 0;
      current_state              = normal;
    } else {
      escape_buffer[eb_cursor++] = c;
    }
    break;
  case escaped:
    escape_buffer[eb_cursor++] = c;
    if (c >= 48 && c <= 57) { // 0-9
      escape_buffer[eb_cursor] = '\0';
      eb_cursor                = 0;
      current_state            = normal;
    } else if (c >= 97 && c <= 122) { // lowercase letters
      escape_buffer[eb_cursor] = '\0';
      eb_cursor                = 0;
      current_state            = normal;
    } else if (c >= 65 && c <= 90) { // uppercase letters
      escape_buffer[eb_cursor] = '\0';
      eb_cursor                = 0;
      current_state            = normal;
    } else if (c == 91) { // starts a longer escape
      current_state = extended1;
    } else { // unsupported escape, reset state
      eb_cursor     = 0;
      current_state = normal;
    }
    break;
  }
}

sm_parse_result sm_terminal_prompt() {
  struct termios old_attr, new_attr;

  // Save current terminal attributes
  tcgetattr(STDIN_FILENO, &old_attr);
  new_attr = old_attr;

  // Set the terminal to non-canonical mode
  new_attr.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &new_attr);

  // Print the prompt
  char prompt_buffer[16];
  snprintf(prompt_buffer, 16, "\n%i> ", yylineno);
  printf("%s", prompt_buffer);
  fflush(stdout);

  // Read input character by character
  char c;
  // read returns # of bytes read or -1
  while (read(STDIN_FILENO, &c, 1) > 0 && c != 10) {
    process_character(c);
  }

  // Restore the original terminal attributes
  tcsetattr(STDIN_FILENO, TCSANOW, &old_attr);
  input_buffer[input_len] = '\0';
  putc('\n', stdout);

  sm_parse_result pr = sm_parse_cstr(input_buffer, input_len);

  input_len = 0;
  return pr;
}
