// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

typedef struct sm_array {
  int16_t  my_type;
  uint16_t inner_type;
  uint32_t size;
} sm_array;

sm_array *sm_new_array(uint16_t type, uint32_t size);
