// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"

sm_bc_block *sm_new_bc_block(sm_cx *meta, int size, char *content) {
  sm_bc_block *new_block = sm_malloc(sizeof(sm_bc_block) + size);
  new_block->my_type     = SM_BC_BLOCK_TYPE;
  new_block->metadata    = meta;
  sm_strncpy(&new_block->content, content, size);
  return new_block;
}
