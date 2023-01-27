// The following file is provided under the BSD 2-clause license. For more info, read LICENSE.txt.

// size = stack->top - &(stack[1])
typedef struct sm_stack {
  unsigned int capacity;
  void       **top;
} sm_stack;

sm_stack    *sm_new_stack(unsigned int capacity);
void       **sm_stack_content(sm_stack *self);
sm_stack    *sm_stack_push(sm_stack *self, void *ptr);
sm_stack    *sm_stack_pop(sm_stack *self);
unsigned int sm_stack_size(sm_stack *self);
void       **sm_stack_empty_top(sm_stack *self);
