// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

/// Structure to hold global info affected by command line arguments
/// @see sm_init populates this struct
typedef struct sm_env {
  /// Version string. Major.Minor.Patch
  char version[17];
  /// Length of version string
  int version_len;
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
  /// Number of bytes specified through -m
  double mem_bytes;
  /// The string parsed to to mem_bytes
  char mem_str[16];
  /// Whether sm_init has completed
  bool initialized;
  /// Whether -q was specified
  bool quiet_mode;
  /// Number of args passed to the main function
  int num_args;
  /// Args passed to the main function
  char **args;
  /// -l none will set this to true in sm_init
  bool no_history_file;
  /// History file path for linenoise
  char history_file[256];
  /// Length of History file path for linenoise
  int history_file_len;
  /// Plain terminal mode
  bool plain_mode;
} sm_env;
