// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

// Typical stack object
typedef struct sm_object_stack {
  uint32_t my_type;
  uint32_t capacity;
  uint32_t size;
  uint32_t filler;
} sm_object_stack;

/// Create a new stack
sm_object_stack *sm_new_object_stack(uint32_t capacity);
/// Return first element of the stack
void **sm_object_stack_content(sm_object_stack *self);
/// Push a pointer onto the stack
sm_object_stack *sm_object_stack_push(sm_object_stack *self, void *ptr);
/// Push a pointer onto the stack
sm_object_stack *sm_object_stack_pop(sm_object_stack *self);
