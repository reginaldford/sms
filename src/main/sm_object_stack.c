// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

// Implements a pointer stack as sms heap object
// The stack will grow arbitrarily large upon continuos pushing.
// New capacity is sm_global_growth_factor*old_capacity + 1

#include "sms.h"

// New stack (is an object) with size 0
sm_object_stack *sm_new_object_stack(uint32_t capacity) {
  sm_object_stack *new_stack = sm_malloc(sizeof(sm_object_stack) + sizeof(sm_object *) * capacity);
  new_stack->my_type         = SM_OBJECT_STACK_TYPE;
  new_stack->capacity        = capacity;
  new_stack->size            = 0;
  return new_stack;
}

void **sm_object_stack_content(sm_object_stack *self) { return (void **)&(self[1]); }

sm_object_stack *sm_object_stack_push(sm_object_stack *self, void *ptr) {
  sm_object_stack *current_stack = self;
  if (self->size == self->capacity) {
    uint32_t old_size = self->capacity;
    self->capacity    = self->capacity * sm_global_growth_factor(0) + 1;
    current_stack     = sm_malloc(sizeof(sm_object_stack) + sizeof(void *) * self->capacity);
    memcpy(current_stack, self, sizeof(sm_object_stack) + sizeof(void *) * old_size);
  }
  void **tuple               = sm_object_stack_content(current_stack);
  tuple[current_stack->size] = ptr;
  current_stack->size += 1;
  return current_stack;
}

sm_object_stack *sm_object_stack_pop(sm_object_stack *self) {
  self->size--;
  return self;
}

// Special pointer location relative to the stack object
// which means that the stack is empty.
void **sm_object_stack_empty_top(sm_object_stack *self) { return ((void **)&(self[1])) - 1; }
