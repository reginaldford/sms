// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

/// An object representing memory space
typedef struct sm_space {
  uint32_t my_type;
  uint32_t size;
} sm_space;

sm_space *sm_new_space(uint32_t size);
