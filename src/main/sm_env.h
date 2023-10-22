// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

/// Structure to hold global info affected by command line arguments
/// @see sm_init populates this struct
typedef struct sm_env {
  /// File path to specified script (if path is given)
  char script_fp[256];
  /// Length of the file path
  short script_fp_len;
  /// Command to evaluate (if -e option is used)
  char eval_cmd[256];
  /// Length of command to evaluate
  short eval_cmd_len;
  /// Whether to gc (hardcoded to true for now)
  bool gc;
  /// Whether custom memory size was specified (-m)
  bool mem_flag;
  /// Number of megabytes specified through -m
  double mem_mbytes;
  /// The string parsed to to mem_bytes
  char mem_str[10];
  /// Whether sm_init has completed
  bool initialized;
  /// Whether -q was specified
  bool quiet_mode;
  /// Number of args passed to the main function
  int num_args;
  /// Args passed to the main function
  char **args;
} sm_env;
