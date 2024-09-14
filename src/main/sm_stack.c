// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

// Implements a pointer stack that lives in OS malloc space.
// The stack will grow arbitrarily large upon continuous pushing.
// New capacity at growth is sm_global_growth_factor * old_capacity + 1

#include "sms.h"

// New stack (not an object)
sm_stack *sm_new_stack(uint32_t capacity) {
  sm_stack *new_stack = malloc(sizeof(sm_stack) + sizeof(void *) * capacity);
  new_stack->capacity = capacity;
  new_stack->top      = sm_stack_empty_top(new_stack);
  return new_stack;
}

void **sm_stack_content(sm_stack *self) { return (void **)&(self[1]); }

sm_stack *sm_stack_push(sm_stack *self, void *ptr) {
  sm_stack *current_stack = self;
  if (sm_stack_size(self) == self->capacity) {
    uint32_t old_size = self->capacity;
    self->capacity    = self->capacity * sm_global_growth_factor(0) + 1;
    current_stack     = malloc(sizeof(sm_stack) + sizeof(void *) * self->capacity);
    memcpy(current_stack, self, sizeof(sm_stack) + sizeof(void *) * old_size);
    free(self);
    current_stack->top = sm_stack_empty_top(current_stack) + old_size;
  }
  void **tuple                        = sm_stack_content(current_stack);
  tuple[sm_stack_size(current_stack)] = ptr;
  current_stack->top += 1;
  return current_stack;
}

void *sm_stack_pop(sm_stack *self) {
  if (sm_stack_size(self) == 0) {
    return NULL;
  }
  self->top--;
  return *(self->top);
}

// Special pointer location relative to the stack object
// which means that the stack is empty.
void **sm_stack_empty_top(sm_stack *self) { return ((void **)&(self[1])) - 1; }

uint32_t sm_stack_size(sm_stack *self) { return ((void **)self->top) - sm_stack_empty_top(self); }
