// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

// Implements a pointer stack as sms heap object
// The stack will grow arbitrarily large upon continuos pushing.
// New capacity is sm_global_growth_factor*old_capacity + 1

#include "sms.h"
extern sm_stack2 *sms_stack;

// New stack (is an object) with size 0
sm_stack2 *sm_new_stack2(uint32_t capacity) {
  sm_stack2 *new_stack2 = malloc(sizeof(sm_stack2) + sizeof(sm_object *) * capacity);
  new_stack2->my_type   = SM_STACK2_TYPE;
  new_stack2->capacity  = capacity;
  new_stack2->size      = 0;
  return new_stack2;
}

inline sm_object **sm_stack2_content() { return (sm_object **)&(sms_stack[1]); }

void sm_push(sm_object *ptr) {
  sm_stack2 *current_stack = sms_stack;
  if (sms_stack->size == sms_stack->capacity) {
    uint32_t old_size   = sms_stack->capacity;
    sms_stack->capacity = sms_stack->capacity * sm_global_growth_factor(0) + 1;
    current_stack       = malloc(sizeof(sm_stack2) + sizeof(void *) * sms_stack->capacity);
    memcpy(current_stack, sms_stack, sizeof(sm_stack2) + sizeof(void *) * old_size);
  }
  sm_object **tuple          = sm_stack2_content();
  tuple[current_stack->size] = ptr;
  current_stack->size += 1;
}

sm_object *sm_pop() {
  if (!sms_stack->size) {
    printf("stack underflow\n");
    return (sm_object *)sms_false;
  }
  sm_object *to_return = sm_stack2_content()[sms_stack->size - 1];
  sms_stack->size--;
  return to_return;
}
