// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

// Structure to hold global info affected by command line arguments
typedef struct sm_options {
  bool         script_flag;
  char         script_fp[256];
  short        script_fp_len;
  bool         init_flag;
  char         init_fp[256];
  short        init_fp_len;
  bool         eval_flag;
  char         eval_cmd[256];
  short        eval_cmd_len;
  bool         gc_flag;
  bool         gc;
  bool         mem_flag;
  unsigned int mem_mbytes;
  char         mem_str[10];
  bool         print_stats;
} sm_options;

sm_options *sm_process_args(int num_args, char **argv);
