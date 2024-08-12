// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

/// Newly implemented bytecode block. incomplete
typedef struct sm_bc_block {
  uint32_t my_type;
  sm_cx   *metadata;
  uint32_t size;
  char     content;
} sm_bc_block;

sm_bc_block *sm_new_bc_block(sm_cx *meta, int size, char *content);
