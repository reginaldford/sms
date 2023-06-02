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

#define MAX_BUFFER_LEN 1024
int               cursor_pos    = 0;
int               input_len     = 0;
sm_terminal_state current_state = normal;       // State machine parsing
char              input_buffer[MAX_BUFFER_LEN]; // Might have to use OS malloc to improve
char              escape_buffer[16];            // For escape codes
int               eb_cursor = 0;                // Escape buffer cursor

// State machine based on sm_terminal_state enum
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

char last_non_whitespace(char *cstr, int len) {
  for (int i = len - 1; i >= 0; i--) {
    if (!isspace(cstr[i]))
      return cstr[i];
  }
  return ' '; // As long as it is not a semicolon,
  // this is not a complete command.
}

// Prints the terminal prompt, and allows the user to input a command
// Parses the input when enter is pressed and the last token is a ;
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

  char c;
  // read returns # of bytes read or -1
  // We read 1 byte at a time
  while (read(STDIN_FILENO, &c, 1) > 0) {
    process_character(c);
    if (c == 10) {
      if (last_non_whitespace(input_buffer, input_len) == ';') {
        tcsetattr(STDIN_FILENO, TCSANOW, &old_attr); // Restore the original terminal attributes
        input_buffer[input_len] = '\0';              // cstr termination
        sm_parse_result pr      = sm_parse_cstr(input_buffer, input_len); //
        input_len               = 0;
        return pr;
      }
    }
  }
  return (sm_parse_result){.return_val = -1, .parsed_object = NULL};
}
