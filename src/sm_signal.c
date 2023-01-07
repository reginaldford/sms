#include "sms.h"

void sm_signal_handler(int signal_number) {
  switch (signal_number) {
  case SIGABRT:
  case SIGHUP:
  case SIGILL:
  case SIGINT:
  case SIGQUIT:
  case SIGTERM: {
    printf("\n<RECEIVED SIGNAL: %i. EXITING.>\n", signal_number);
    sm_mem_cleanup();
    exit(0);
    break;
  }
  case SIGFPE: {
    printf("\n<RECEIVED FPE SIGNAL.>\n");
  }
  }
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
