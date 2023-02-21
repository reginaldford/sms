// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

// Structure to hold global info affected by command line arguments
typedef struct sm_options {
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
  bool         script_flag;
  char         script_fp[256];
  short        script_fp_len;
} sm_options;
