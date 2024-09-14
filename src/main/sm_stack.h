// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

/// This is a pointer stack I made, optimized for peeking.
/// Instead of an integer for the top location, it uses a pointer
/// Therefore, size = stack->top - &(stack[1])
typedef struct sm_stack {
  uint64_t capacity;
  void   **top;
} sm_stack;

/// Create a new stack
sm_stack *sm_new_stack(uint32_t capacity);
/// Return first element of the stack
void **sm_stack_content(sm_stack *self);
/// Push a pointer onto the stack
sm_stack *sm_stack_push(sm_stack *self, void *ptr);
/// Pop an element off the stack
void *sm_stack_pop(sm_stack *self);
/// Number of elements in the stack
uint32_t sm_stack_size(sm_stack *self);
/// If the stack was empty, the top is a special pointer that goes into the stack's struct
void **sm_stack_empty_top(sm_stack *self);
