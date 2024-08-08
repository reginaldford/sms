// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"

sm_array *sm_new_array(uint16_t type, uint32_t size) {
  sm_array *output =
    sm_malloc(sizeof(sm_array) + (sizeof(type) - sm_type_header_size(type)) * size);
  output->my_type    = SM_ARRAY_TYPE;
  output->inner_type = type;
  output->size       = size;
  return output;
}
