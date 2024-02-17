// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

/// This is a pointer stack I made, optimized for peeking.
/// Instead of an integer for the top location, it uses a pointer
/// Therefore, size = stack->top - &(stack[1])
typedef struct sm_stack_obj {
  int16_t  my_type;
  uint32_t capacity;
  void   **top;
} sm_stack_obj;

/// Create a new stack
sm_stack_obj *sm_new_stack_obj(uint32_t capacity);
/// Return first element of the stack
void **sm_stack_obj_content(sm_stack_obj *self);
/// Push a pointer onto the stack
sm_stack_obj *sm_stack_obj_push(sm_stack_obj *self, void *ptr);
/// Push a pointer onto the stack
sm_stack_obj *sm_stack_obj_pop(sm_stack_obj *self);
/// Push a pointer onto the stack
uint32_t sm_stack_obj_size(sm_stack_obj *self);
/// If the stack was empty, the top is a special pointer that goes into the stack's struct
void **sm_stack_obj_empty_top(sm_stack_obj *self);
