// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

/// Structure to hold global info affected by command line arguments
typedef struct sm_env {
  bool   script_flag;
  char   script_fp[256];
  short  script_fp_len;
  bool   eval_flag;
  char   eval_cmd[256];
  short  eval_cmd_len;
  bool   gc_flag;
  bool   gc;
  bool   mem_flag;
  double mem_mbytes;
  char   mem_str[10];
  bool   initialized;
  bool   quiet_mode;
  int    num_args;
  char **args;
} sm_env;
