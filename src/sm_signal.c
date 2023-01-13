// The following file is provided under the BSD 2-clause license. For more info, read LICENSE.txt.
#include "sms.h"

void sm_signal_handler(int signal_number) {
  char *signal_name = "";
  switch (signal_number) {
  case SIGQUIT:
    signal_name = "SIGQUIT";
  case SIGHUP:
    signal_name = "SIGHUP";
  case SIGABRT:
    signal_name = "SIGABRT";
  case SIGILL:
    signal_name = "SIGILL";
  case SIGINT:
    signal_name = "SIGINT";
  case SIGTERM: {
    signal_name = "SIGTERM";
    printf("\n<Received signal: %s. Exiting.>\n", signal_name);
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
