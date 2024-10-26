// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"

extern void **memory_marker1;
extern void **memory_marker2;
extern bool   evaluating;

// Copy the object
sm_object *sm_copy(sm_object *obj) {
  if (obj->my_type != SM_SYMBOL_TYPE)
    return sm_realloc(obj, sm_sizeof(obj));
  else
    return obj;
}

// Deep Copy the object
sm_object *sm_deep_copy(sm_object *obj) {
  if (obj->my_type == SM_EXPR_TYPE) {
    sm_expr *expr = (sm_expr *)sm_copy(obj);
    for (uint32_t i = 0; i < expr->size; i++)
      sm_expr_set_arg(expr, i, sm_deep_copy(sm_expr_get_arg(expr, i)));
    return obj;
  } else {
    sm_object *new_obj = sm_realloc(obj, sm_sizeof(obj));
    return new_obj;
  }
}

// Copy the object to the new heap
// Leave an sm_pointer in the old space
sm_object *sm_move_to_new_heap(sm_heap *dest, sm_object *obj) {
  uint32_t   sizeOfObj = sm_sizeof(obj);
  sm_object *new_obj   = sm_realloc_at(dest, obj, sizeOfObj);
  // Overwrite the old object. sm_pointer objects
  intptr_t endPoint = (intptr_t)obj + sizeOfObj;
  sm_new_pointer(dest, obj, new_obj);
  return new_obj;
}

// If obj is an sm_pointer, the object was already moved to the new heap
// Else if the object is in the source heap, copy the object to the desth heap and leave an
// sm_pointer Expects sm_heap_scan to be used on this to have populated map
sm_object *sm_meet_object(sm_heap *source, sm_heap *dest, sm_object *obj) {
  // Only gc objects from source
  if (sm_heap_has_object(source, obj)) {
    uint32_t obj_type = obj->my_type;
    if (obj_type == SM_POINTER_TYPE)
      return (sm_object *)(((uint64_t)dest) + (uint64_t)((sm_pointer *)obj)->address);
    else
      return sm_move_to_new_heap(dest, obj);
  } else
    return obj;
}

// Copy the objects referenced by the current_obj into the new heap
// and copy all referenced objects until all possible references are copied
void sm_inflate_heap(sm_heap *from, sm_heap *to) {
  // Inflate new space. 'meet' every ptr
  // Meeting will copy to new heap if necessary
  char *scan_cursor = (char *)to->storage;
  while (scan_cursor < to->storage + to->used) {
    sm_object *current_obj = (sm_object *)scan_cursor;
    // Check sizeof, avoid an infinite loop
    size_t obj_size = sm_sizeof(current_obj);
    if (!obj_size) {
      fprintf(stderr, "Error: Cannot determine object size.\n");
      exit(1);
    }
    // scan_cursor is not referred to for the rest of the loop
    scan_cursor += obj_size;
    switch (current_obj->my_type) {
    case SM_CX_TYPE: {
      sm_cx *cx = (sm_cx *)current_obj;
      // Meet the parent
      if (cx->parent)
        cx->parent = (sm_cx *)sm_meet_object(from, to, (sm_object *)cx->parent);
      // Meet the top cx node
      if (cx->content)
        cx->content = (sm_node *)sm_meet_object(from, to, (sm_object *)cx->content);
      break;
    }
    case SM_NODE_TYPE: {
      sm_node *node = (sm_node *)current_obj;
      // Meet the value
      if (node->value)
        node->value = sm_meet_object(from, to, (sm_object *)node->value);
      // Meet the next node
      if (node->next)
        node->next = (struct sm_node *)sm_meet_object(from, to, (sm_object *)node->next);
      // Meet the children
      if (node->children)
        node->children = (struct sm_node *)sm_meet_object(from, to, (sm_object *)node->children);
      break;
    }
    case SM_EXPR_TYPE: {
      sm_expr *expr = (sm_expr *)current_obj;
      if (expr->notes)
        expr->notes = sm_meet_object(from, to, expr->notes);
      for (uint32_t i = 0; i < expr->size; i++) {
        sm_object *new_obj = sm_meet_object(from, to, sm_expr_get_arg(expr, i));
        sm_expr_set_arg(expr, i, (sm_object *)new_obj);
      }
      break;
    }
    case SM_META_TYPE: {
      sm_meta *meta = (sm_meta *)current_obj;
      meta->address = (sm_object *)sm_meet_object(from, to, meta->address);
      break;
    }
    case SM_FUN_TYPE: {
      sm_fun *fun  = (sm_fun *)current_obj;
      fun->content = sm_meet_object(from, to, (sm_object *)fun->content);
      if (fun->parent)
        fun->parent = (sm_cx *)sm_meet_object(from, to, (sm_object *)fun->parent);
      for (uint32_t i = 0; i < fun->num_params; i++) {
        sm_fun_param *param = sm_fun_get_param(fun, i);
        param->name         = (sm_string *)sm_meet_object(from, to, (sm_object *)param->name);
        if (param->default_val) {
          param->default_val = sm_meet_object(from, to, param->default_val);
        }
      }
      break;
    }
    case SM_ERR_TYPE: {
      sm_error *err = (sm_error *)current_obj;
      err->message  = (sm_string *)sm_meet_object(from, to, (sm_object *)err->message);
      err->source   = (sm_string *)sm_meet_object(from, to, (sm_object *)err->source);
      if (err->notes)
        err->notes = (sm_cx *)sm_meet_object(from, to, (sm_object *)err->notes);
      break;
    }
    case SM_FUN_PARAM_TYPE: {
      sm_fun_param_obj *param = (sm_fun_param_obj *)current_obj;
      param->name             = (sm_string *)sm_meet_object(from, to, (sm_object *)param->name);
      if (param->default_val) {
        param->default_val = sm_meet_object(from, to, (sm_object *)param->default_val);
      }
      break;
    }
    case SM_LOCAL_TYPE: {
      sm_local *local = (sm_local *)current_obj;
      local->name     = (sm_string *)sm_meet_object(from, to, (sm_object *)local->name);
      break;
    }
    case SM_RETURN_TYPE: {
      sm_return *return_obj = (sm_return *)current_obj;
      return_obj->address   = sm_meet_object(from, to, (sm_object *)return_obj->address);
      break;
    }
    case SM_STACK_OBJ_TYPE: {
      sm_stack_obj *stack = (sm_stack_obj *)current_obj;
      for (uint32_t i = 0; i < stack->size; i++)
        ((sm_object **)&stack[1])[i] = sm_meet_object(from, to, ((sm_object **)&stack[1])[i]);
      break;
    }
    case SM_ARRAY_TYPE: {
      sm_array *a = (sm_array *)current_obj;
      a->content  = sm_meet_object(from, to, a->content);
      break;
    }
    default:
      break;
    }
  }
}

// Copying GC
void sm_garbage_collect() {
  if (!sms_heap->used)
    return;
  // Fill in the heap map
  if (evaluating && !sm_heap_scan(sms_heap)) {
    fprintf(stderr, "Heap scan failed. Exiting with code 1\n");
    exit(1);
  }
  //  Build "to" heap if necessary, same size as current
  if (!sms_other_heap)
    sms_other_heap = sm_new_heap(sms_heap->capacity, true);
  // Clear other heap for when we recycle a heap
  sm_heap_clear(sms_other_heap);
  // Try to shake off objects from callstack with unregistered spacer
  if (evaluating)
    sm_new_space_at(sms_other_heap, ((sms_heap > sms_other_heap) + (sm_gc_count(0) & 7)) << 3);
  // Fix c callstack ptrs if evaluating
  if (evaluating) {
    memory_marker2   = __builtin_frame_address(0);
    void **lowerPtr  = memory_marker1 < memory_marker2 ? memory_marker1 : memory_marker2;
    void **higherPtr = memory_marker1 < memory_marker2 ? memory_marker2 : memory_marker1;
    for (void **ptr = (void **)(((uintptr_t)lowerPtr) & ~7); ptr < higherPtr; ptr++)
      if (sm_heap_has_object(sms_heap, *ptr)) {
        sm_object *obj = *ptr;
        // Bravely clean the runtime callstack
        if (!sm_sizeof(obj))
          *ptr = NULL;
        else
          *ptr = (void *)sm_meet_object(sms_heap, sms_other_heap, (sm_object *)*ptr);
      }
  }
  // Copy root (global context)
  *sm_global_lex_stack(NULL)->top =
    sm_meet_object(sms_heap, sms_other_heap, (sm_object *)*sm_global_lex_stack(NULL)->top);
  // Treat parser output as root or set parser output to false
  if (evaluating)
    sm_global_parser_output(
      sm_meet_object(sms_heap, sms_other_heap, sm_global_parser_output(NULL)));
  else
    sm_global_parser_output((sm_object *)sms_false);
  // Inflate
  sm_inflate_heap(sms_heap, sms_other_heap);
  // For tracking purposes
  sm_gc_count(1);
  // swap global heap ptrs
  sm_swap_heaps(&sms_heap, &sms_other_heap);
  // Report memory stat
  if (sm_global_environment(NULL) && sm_global_environment(NULL)->quiet_mode == false) {
    char used_str[16];
    char capacity_str[16];
    sm_sprint_fancy_bytelength(used_str, (uint64_t)sms_heap->used);
    sm_sprint_fancy_bytelength(capacity_str, (uint64_t)sms_heap->capacity);
    printf("\n%s(%s / %s)%s\n", sm_terminal_fg_color(SM_TERM_B_BLACK), used_str, capacity_str,
           sm_terminal_reset());
  }
}
