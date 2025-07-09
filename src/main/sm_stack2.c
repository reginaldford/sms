// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

// Implements a pointer stack as sms heap object
// The stack will grow arbitrarily large upon continuos pushing.
// New capacity is sm_global_growth_factor*old_capacity + 1

#include "sms.h"

// New stack (is an object) with size 0
sm_stack2 *sm_new_stack2(uint32_t capacity) {
  sm_stack2 *new_stack2 = malloc(sizeof(sm_stack2) + sizeof(sm_object *) * capacity);
  new_stack2->my_type   = SM_STACK2_TYPE;
  new_stack2->capacity  = capacity;
  new_stack2->size      = 0;
  return new_stack2;
}

inline sm_object **sm_stack2_content(sm_stack2 *stack) { return (sm_object **)&(stack[1]); }

sm_stack2 *sm_push(sm_stack2 *stack, sm_object *ptr) {
  sm_stack2 *current_stack = stack;
  if (stack->size == stack->capacity) {
    uint32_t old_size = stack->capacity;
    // Increase the capacity and check for overflow
    stack->capacity = stack->capacity * sm_global_growth_factor(0) + 1;
    if (stack->capacity < old_size) {
      // Handle capacity overflow if needed
      return NULL; // or some error handling
    }
    // Allocate new stack structure with extra room for the content array
    current_stack = malloc(sizeof(sm_stack2) + sizeof(void *) * stack->capacity);
    if (!current_stack) {
      // Handle malloc failure
      return NULL;
    }
    // Copy relevant fields from old stack to new stack
    memcpy(current_stack, stack,
           sizeof(sm_stack2)); // Copy the base structure (size, capacity, etc.)
    // Copy the old stack content into the new one
    sm_object **old_content = sm_stack2_content(stack);
    sm_object **new_content = sm_stack2_content(current_stack);
    memcpy(new_content, old_content, sizeof(void *) * old_size);
    // Free old stack after copying content
    free(stack);
  }
  // Push the new object onto the stack
  sm_object **tuple          = sm_stack2_content(current_stack);
  tuple[current_stack->size] = ptr;
  current_stack->size += 1;
  return current_stack;
}


sm_object *sm_pop(sm_stack2 *stack) {
  if (!stack->size) {
    printf("stack underflow %s:%i \n", __FILE__, __LINE__);
    return (sm_object *)sms_false;
  }
  sm_object *to_return = sm_stack2_content(stack)[stack->size - 1];
  stack->size--;
  return to_return;
}

sm_object *sm_peek(sm_stack2 *stack) {
  if (stack->size == 0) {
    printf("stack underflow %s:%i \n", __FILE__, __LINE__);
    return (sm_object *)sms_false;
  }
  sm_object *to_return = sm_stack2_content(stack)[stack->size - 1];
  return to_return;
}
sm_object *sm_peek2(sm_stack2 *stack) {
  if (stack->size < 2) {
    printf("stack underflow %s:%i \n", __FILE__, __LINE__);
    return (sm_object *)sms_false;
  }
  sm_object *to_return = sm_stack2_content(stack)[stack->size - 2];
  return to_return;
}
sm_object *sm_peek3(sm_stack2 *stack) {
  if (stack->size < 3) {
    printf("stack underflow %s:%i \n", __FILE__, __LINE__);
    return (sm_object *)sms_false;
  }
  sm_object *to_return = sm_stack2_content(stack)[stack->size - 3];
  return to_return;
}
