// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

// Typical stack object
typedef struct sm_stack_obj {
  uint32_t my_type;
  uint32_t capacity;
  uint32_t size;
  uint32_t filler;
} sm_stack_obj;

/// Create a new stack
sm_stack_obj *sm_new_stack_obj(uint32_t capacity);
/// Return first element of the stack
void **sm_stack_obj_content(sm_stack_obj *self);
/// Push a pointer onto the stack
sm_stack_obj *sm_stack_obj_push(sm_stack_obj *self, void *ptr);
/// Push a pointer onto the stack
sm_stack_obj *sm_stack_obj_pop(sm_stack_obj *self);
