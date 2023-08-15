// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

typedef struct sm_bc_block {
  unsigned short int my_type;
  sm_cx             *metadata;
  unsigned int       size;
  char               content;
} sm_bc_block;

sm_bc_block *sm_new_bc_block(sm_cx *meta, int size, char *content);
