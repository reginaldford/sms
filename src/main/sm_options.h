// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

// Structure to hold global info affected by command line arguments
typedef struct sm_options {
  bool         script_flag;
  char         script_fp[256];
  short        script_fp_len;
  bool         env_flag;
  char         env_fp[256];
  short        env_fp_len;
  bool         init_flag;
  char         init_fp[256];
  short        init_fp_len;
  bool         eval_flag;
  char         eval_cmd[256];
  short        eval_cmd_len;
  bool         always_gc;
  bool         mem_flag;
  unsigned int memory_bytes;
  unsigned int memory_unit;
  bool         print_stats;
} sm_options;

sm_options *sm_process_args(int num_args, char **argv);
