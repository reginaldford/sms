// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

/// POSIX signal handler
void sm_signal_handler(int32_t signal_number);
/// Designates sm_signal_handler for many signals
void sm_register_signals();
/// Gracefully exit with exit_code
void sm_signal_exit(int32_t exit_code);
