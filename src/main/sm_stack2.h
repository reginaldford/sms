// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

// Typical stack object
typedef struct sm_stack2 {
  uint32_t my_type;
  uint32_t capacity;
  uint32_t size;
  uint32_t filler;
} sm_stack2;

/// Create a new stack
sm_stack2 *sm_new_stack2(uint32_t capacity);
/// Return first element of the stack
sm_object **sm_stack2_content(sm_stack2 *stack);
/// Push a pointer onto the stack
sm_stack2 *sm_push(sm_stack2 *stack, sm_object *ptr);
/// Push a pointer onto the stack
sm_object *sm_pop(sm_stack2 *stack);
/// Get the last pushed object or return NULL
sm_object *sm_peek(sm_stack2 *stack);
sm_object *sm_peek2(sm_stack2 *stack);
sm_object *sm_peek3(sm_stack2 *stack);
