// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "sms.h"
extern bool  evaluating;
extern void *memory_marker1;

void start_repl(sm_env *env);

void sm_default_signal_handler(int signal_number) {
  char    *signal_name = "";
  uint16_t exit_code;
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
    if (evaluating) {
      evaluating = false;
      sm_global_parser_output((sm_object *)sms_false);
      sigset_t set;
      sigemptyset(&set);
      sigaddset(&set, SIGINT);
      sigprocmask(SIG_UNBLOCK, &set, NULL);
      // Let's set the top of the callstack here (growing downward)
      memory_marker1 = __builtin_frame_address(0);
      start_repl(sm_global_environment(NULL));
      return;
    } else {
      signal_name = "SIGINT";
      exit_code   = 0;
    }
    break;
  case SIGTERM:
    signal_name = "SIGTERM";
    exit_code   = 128 + 0;
    break;
  case SIGFPE:
    signal_name = "SIGFPE";
    exit_code   = 128 + SIGFPE;
    break;
  case SIGSEGV:
    signal_name = "SIGSEGV";
    exit_code   = 128 + SIGFPE;
    sm_dump_and_count();
    printf("dumped memory.\n");
    break;
  default:
    signal_name = "unknown signal";
    exit_code   = 1;
    break;
  }
  if (sm_global_environment(NULL)->quiet_mode == false)
    printf("\n%s<Received signal: %s. Exiting with code: %i>\n",
           sm_terminal_fg_color(SM_TERM_YELLOW), signal_name, exit_code);
  printf("%s", sm_terminal_reset());
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

void sm_signal_handler(int signal_number) {
  // TODO: change to sms_scratch global
  sm_cx   *scratch = *sm_global_lex_stack(NULL)->top;
  sm_fun  *fun     = (sm_fun *)sm_cx_get_far(scratch, sm_new_symbol("_sigHandler", 11));
  sm_expr *sf = sm_new_expr(SM_PARAM_LIST_EXPR, (sm_object *)sm_new_ui8((ui8)signal_number), NULL);
  // _sigHandler(s) , where s is an f64 with the signal number
  if (fun) {
    execute_fun(fun, scratch, sf);
    return;
  } else {
    sm_default_signal_handler(signal_number);
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
  //  Segfault handling
  signal(SIGSEGV, sm_signal_handler);
}

// Exit SMS with this exit code
void sm_signal_exit(int exit_code) {
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
