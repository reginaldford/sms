// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "sms.h"

void sm_signal_handler(int signal_number) {
  char              *signal_name = "";
  unsigned short int exit_code;
  switch (signal_number) {
  case SIGQUIT:
    signal_name = "SIGQUIT";
    exit_code   = 128 + SIGQUIT;
    break;
  case SIGHUP:
    signal_name = "SIGHUP";
    exit_code   = 128 + SIGHUP;
    break;
  case SIGABRT:
    signal_name = "SIGABRT";
    exit_code   = 128 + SIGABRT;
    break;
  case SIGILL:
    signal_name = "SIGILL";
    exit_code   = 128 + SIGILL;
    break;
  case SIGINT:
    signal_name = "SIGINT";
    exit_code   = 128 + SIGINT;
    break;
  case SIGTERM:
    signal_name = "SIGTERM";
    exit_code   = 128 + SIGTERM;
    break;
  case SIGFPE:
    signal_name = "SIGFPE";
    exit_code   = 128 + SIGFPE;
    break;
  default:
    signal_name = "unknown signal";
    exit_code   = 1;
    break;
  }
  if (sm_global_environment(NULL)->quiet_mode == false)
    printf("\n%s<Received signal: %s. Exiting with code: %i>\n",
           sm_terminal_fg_color(SM_TERM_YELLOW), signal_name, exit_code);
  sm_mem_cleanup();
  // Reset the terminal attributes to their default values
  int            stdin_fd = fileno(stdin);
  struct termios term_attr;
  tcgetattr(stdin_fd, &term_attr);
  term_attr.c_lflag |= ICANON | ECHO;
  tcsetattr(stdin_fd, TCSANOW, &term_attr);
  fflush(stdout);
  exit(exit_code);
}

void sm_register_signals() {
  // Abort
  signal(SIGABRT, sm_signal_handler);
  // Hang up (terminal disconnect)
  signal(SIGHUP, sm_signal_handler);
  // Floating point exception
  signal(SIGFPE, sm_signal_handler);
  // Illegal instruction
  signal(SIGILL, sm_signal_handler);
  // Interuption (like ctrl+c)
  signal(SIGINT, sm_signal_handler);
  // Quit
  signal(SIGQUIT, sm_signal_handler);
  // Terminate
  signal(SIGTERM, sm_signal_handler);
  // Trap (for debugger)
  //  signal(SIGTRAP, sm_signal_handler);
}

// Exit SMS with this exit code
void sm_signal_exit(int exit_code) {
  if (sm_global_environment(NULL)->quiet_mode == false) {
    sm_terminal_color exit_color = exit_code ? SM_TERM_YELLOW : SM_TERM_WHITE;
    printf("%s<Exiting with code: %i>%s\n", sm_terminal_fg_color(exit_color), exit_code,
           sm_terminal_reset());
  }
  sm_mem_cleanup();
  // Reset the terminal attributes to their default values
  int            stdin_fd = fileno(stdin);
  struct termios term_attr;
  tcgetattr(stdin_fd, &term_attr);
  term_attr.c_lflag |= ICANON | ECHO;
  tcsetattr(stdin_fd, TCSANOW, &term_attr);
  fflush(stdout);
  exit(exit_code);
}
