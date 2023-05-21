// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "sms.h"

void sm_signal_handler(int signal_number) {
  char *signal_name = "";
  int   exit_code;
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
    printf("\n<Received signal: %s. Exiting with code: %i>\n", signal_name, exit_code);
  sm_mem_cleanup();
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
  sm_mem_cleanup();
  if (sm_global_environment(NULL)->quiet_mode == false)
    printf("<Exiting with code: %i>\n", exit_code);
  fflush(stdout);
  exit(exit_code);
}
