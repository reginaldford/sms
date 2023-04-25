// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "sms.h"

// New stack (not an object)
sm_stack *sm_new_stack(unsigned int capacity) {
  sm_stack *new_stack = malloc(sizeof(sm_stack) + sizeof(sm_cx *) * capacity);
  new_stack->capacity = capacity;
  new_stack->top      = sm_stack_empty_top(new_stack);
  return new_stack;
}

void **sm_stack_content(sm_stack *self) { return (void **)&(self[1]); }

sm_stack *sm_stack_push(sm_stack *self, void *ptr) {
  sm_stack *current_stack = self;
  if (sm_stack_size(self) == self->capacity) {
    unsigned int old_size = self->capacity;
    self->capacity        = self->capacity * sm_global_growth_factor(0) + 1;
    current_stack         = malloc(sizeof(sm_stack) + sizeof(void *) * self->capacity);
    memcpy(current_stack, self, sizeof(sm_stack) + sizeof(void *) * old_size);
  }
  void **array                        = sm_stack_content(current_stack);
  array[sm_stack_size(current_stack)] = ptr;
  current_stack->top += 1;
  return current_stack;
}

sm_stack *sm_stack_pop(sm_stack *self) {
  void *previous_top_obj = *(self->top);
  if (self->top != NULL) {
    self->top--;
    return previous_top_obj;
  }
  printf("stack underflow occurred\n");
  return previous_top_obj;
}

// Special pointer location relative to the stack object
// which means that the stack is empty.
void **sm_stack_empty_top(sm_stack *self) { return ((void **)&(self[1])) - 1; }

unsigned int sm_stack_size(sm_stack *self) {
  if (self->top == sm_stack_empty_top(self))
    return 0;
  return ((void **)self->top) - (void **)&(self[1]) + 1;
}
