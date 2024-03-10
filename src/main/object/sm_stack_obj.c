// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

// Implements a pointer stack as sms heap object
// The stack will grow arbitrarily large upon continuos pushing.
// New capacity is sm_global_growth_factor*old_capacity + 1

#include "../sms.h"

// New stack (is an object)
sm_stack_obj *sm_new_stack_obj(uint32_t capacity) {
  sm_stack_obj *new_stack = sm_malloc(sizeof(sm_stack_obj) + sizeof(sm_object *) * capacity);
  new_stack->my_type      = SM_STACK_OBJ_TYPE;
  new_stack->capacity     = capacity;
  new_stack->top          = sm_stack_obj_empty_top(new_stack);
  return new_stack;
}

void **sm_stack_obj_content(sm_stack_obj *self) { return (void **)&(self[1]); }

sm_stack_obj *sm_stack_obj_push(sm_stack_obj *self, void *ptr) {
  sm_stack_obj *current_stack = self;
  if (sm_stack_obj_size(self) == self->capacity) {
    uint32_t old_size = self->capacity;
    self->capacity    = self->capacity * sm_global_growth_factor(0) + 1;
    current_stack     = sm_malloc(sizeof(sm_stack_obj) + sizeof(void *) * self->capacity);
    memcpy(current_stack, self, sizeof(sm_stack_obj) + sizeof(void *) * old_size);
  }
  void **array                            = sm_stack_obj_content(current_stack);
  array[sm_stack_obj_size(current_stack)] = ptr;
  current_stack->top += 1;
  return current_stack;
}

sm_stack_obj *sm_stack_obj_pop(sm_stack_obj *self) {
  void *previous_top_obj = *(self->top);
  self->top--;
  return previous_top_obj;
}

// Special pointer location relative to the stack object
// which means that the stack is empty.
void **sm_stack_obj_empty_top(sm_stack_obj *self) { return ((void **)&(self[1])) - 1; }

uint32_t sm_stack_obj_size(sm_stack_obj *self) {
  return ((void **)self->top) - sm_stack_obj_empty_top(self);
}
