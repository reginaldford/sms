// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

typedef struct sm_bstack {
  unsigned int capacity;
  unsigned int top;
  char *data;
} sm_bstack;

void sm_op_push(uint8_t value);
uint8_t sm_op_pop();
void sm_op_push2(uint16_t value);
uint16_t sm_op_pop2();
void sm_op_push4(uint32_t value);
uint32_t sm_op_pop4();
void sm_op_push8(uint64_t value);
uint64_t sm_op_pop8();
uint8_t sm_op_peek();
