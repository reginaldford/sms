// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"

/* Relies on comformity to POSIX and vt100/ANSI color code output.
The terminal input is handled by switching the terminal to non-canonical mode. In this mode,
character printing is manual (even when the user types). A state machine in this file will sort the
different incoming escape codes due to standard unix-like behavior on a vt100-like terminal emulator
when the user presses certain key combinations or keys. The state machine parses the input in real
time, and calls the functions for escape codes, extended escape codes, and extra-extended escape
codes which each have their own file.*/

// arrow keys, alt+letters
#include "process_esc.h"
// ctrl+letter
#include "process_ext1.h"
// alt+arrow, ctrl+arrow, ctrl+alt+arrow, ctrl+shift+arrow
#include "process_ext2.h"

// bison generated parser's global line number :-(
extern int yylineno;

// levels of input state
typedef enum { normal, esc, extended1, extended2 } sm_terminal_state;

// we will make this infinite later, via OS malloc
#define MAX_BUFFER_LEN 1024
int               cursor_pos    = 0;
int               input_len     = 0;
sm_terminal_state current_state = normal;       // State machine parsing
char              input_buffer[MAX_BUFFER_LEN]; // Might have to use OS malloc to improve
char              esc_buffer[16];               // For esc codes
int               eb_cursor = 0;                // Escape buffer cursor

// State machine based on sm_terminal_state enum
void process_character(char c) {
  switch (current_state) {
  case normal:
    if (c == 27) { // begins escape code
      current_state = esc;
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
  case extended1: // c is last char or this is an extended esc
    esc_buffer[eb_cursor++] = c;
    if (c == 49) {
      current_state = extended2;
    } else {
      esc_buffer[eb_cursor++] = '\0';
      current_state           = normal;
      eb_cursor               = 0;
      process_ext1(esc_buffer);
    }
    break;
  case extended2:
    if (eb_cursor >= 4) { // extended sequence is 5 total characters
      esc_buffer[eb_cursor++] = c;
      esc_buffer[eb_cursor++] = '\0';
      eb_cursor               = 0;
      current_state           = normal;
      process_ext2(esc_buffer);
    } else {
      esc_buffer[eb_cursor++] = c;
    }
    break;
  case esc:
    esc_buffer[eb_cursor++] = c;
    if (c >= 48 && c <= 57) { // 0-9
      esc_buffer[eb_cursor] = '\0';
      eb_cursor             = 0;
      process_esc(input_buffer);
      current_state = normal;
    } else if (c >= 97 && c <= 122) { // lowercase letters
      esc_buffer[eb_cursor] = '\0';
      eb_cursor             = 0;
      process_esc(input_buffer);
      current_state = normal;
    } else if (c >= 65 && c <= 90) { // uppercase letters
      esc_buffer[eb_cursor] = '\0';
      eb_cursor             = 0;
      process_esc(input_buffer);
      current_state = normal;
    } else if (c == 91) { // starts a longer esc
      current_state = extended1;
    } else { // unsupported esc, reset state
      eb_cursor     = 0;
      current_state = normal;
    }
    break;
  }
}

// Returns the last non-whitespace character of the user input.
// If we reach the beginning in the search, we just return ' '.
char last_non_whitespace(char *cstr, int len) {
  for (int i = len - 1; i >= 0; i--) {
    if (!isspace(cstr[i]))
      return cstr[i];
  }
  return ' ';
}

// "smartput"
// Prints the terminal prompt, and allows the user to input a command
// Parses the input when enter is pressed and the last token is a ;
sm_parse_result sm_terminal_smartput() {
  struct termios term_attr;

  // Set the terminal to non-canonical mode
  tcgetattr(STDIN_FILENO, &term_attr);
  term_attr.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &term_attr);

  sm_terminal_print_prompt();

  char c;
  // read returns # of bytes read or -1
  // We read 1 byte at a time
  while (read(STDIN_FILENO, &c, 1) > 0) {
    process_character(c);
    if (c == 10) {
      if (last_non_whitespace(input_buffer, input_len) != '\\') { // use \ to extend your line
                                                                  // restore terminal attributes
        int            stdin_fd = fileno(stdin);
        struct termios term_attr;
        tcgetattr(stdin_fd, &term_attr);
        term_attr.c_lflag |= ICANON;
        tcsetattr(stdin_fd, TCSANOW, &term_attr);
        input_buffer[input_len] = '\0';                                   // cstr termination
        sm_parse_result pr      = sm_parse_cstr(input_buffer, input_len); //
        input_len               = 0;
        return pr;
      }
    }
  }
  return (sm_parse_result){.return_val = -1, .parsed_object = NULL};
}
