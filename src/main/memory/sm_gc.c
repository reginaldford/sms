// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"

extern void **memory_marker1;
extern void **memory_marker2;
extern bool   evaluating;

bool sm_is_sensible_object(sm_object *obj, sm_heap *location) {
  if (sm_is_within_heap(obj, location) && sm_sizeof(obj) &&
      ((intptr_t)obj) % (sizeof(size_t) / 2) == 0) {
    return true;
  }
  return false;
}


// Copy the object
sm_object *sm_copy(sm_object *obj) {
  // if (sm_is_within_heap(obj,sms_heap))
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
  sm_object *new_obj = sm_realloc_at(dest, obj, sm_sizeof(obj));
  // Overwrite the old object. sm_pointer is NOT larger
  sm_new_pointer(dest, obj, new_obj);
  return new_obj;
}

// If obj is an sm_pointer, the object was already moved to the new heap
// Else, copy the object to the new heap and leave an sm_pointer
sm_object *sm_meet_object(sm_heap *source, sm_heap *dest, sm_object *obj) {
  // Only gc objects from sms_other_heap, which used to be sms_heap
  if (sm_is_sensible_object(obj, source)) {
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
    if (!sm_sizeof(current_obj)) {
      fprintf(stderr, "Error: Cannot determine object size.\n");
      exit(1);
    }
    // scan_cursor is not referred to for the rest of the loop
    scan_cursor += sm_sizeof(current_obj);
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
      for (uint32_t i = 0; i < sm_stack_obj_size(stack); i++)
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
void sm_garbage_collect(sm_heap *from_heap, sm_heap *to_heap) {
  if (from_heap->used != 0) {
    // Build "to" heap if necessary, same size as current
    if (to_heap == NULL)
      to_heap = sm_new_heap(from_heap->capacity);
    // For when we recycle a heap...
    to_heap->used = 0;
    // We need to clear the memory in case of gc during building an object
    memset(to_heap->storage, 0, to_heap->capacity);

    if (evaluating) {
      // Fix c callstack ptrs
      memory_marker2   = __builtin_frame_address(0);
      void **lowerPtr  = memory_marker1 < memory_marker2 ? memory_marker1 : memory_marker2;
      void **higherPtr = memory_marker1 < memory_marker2 ? memory_marker2 : memory_marker1;
      // Alignment
      lowerPtr -= ((intptr_t)lowerPtr) % sizeof(size_t);
      for (void **ptr = lowerPtr; ptr < higherPtr; ptr++) {
        // We are updating pointers in the c callstack. Watch for false alerts.
        if (sm_is_sensible_object((sm_object *)*ptr, from_heap))
          *ptr = (void *)sm_meet_object(from_heap, to_heap, (sm_object *)*ptr);
      }
    }

    // Copy root (global context)
    *sm_global_lex_stack(NULL)->top =
      sm_meet_object(from_heap, to_heap, (sm_object *)*sm_global_lex_stack(NULL)->top);

    // Treat parser output as root
    if (evaluating)
      sm_global_parser_output(sm_meet_object(from_heap, to_heap, sm_global_parser_output(NULL)));
    // Or set parser output to false
    else
      sm_global_parser_output((sm_object *)sms_false);

    // Inflate
    sm_inflate_heap(from_heap, to_heap);

    // For tracking purposes
    sm_gc_count(1);
  }
  // Report memory stat
  FILE *output = sm_global_environment(NULL) && sm_global_environment(NULL)->quiet_mode == false
                   ? stdout
                   : fopen("/dev/null", "w");
  if (output) {
    if (!evaluating)
      putc('\n', output);
    fprintf(output, "%s", sm_terminal_fg_color(SM_TERM_B_BLACK));
    putc('(', output);
    if (output == stdout)
      sm_print_fancy_bytelength((long long)to_heap->used);
    fprintf(output, " / ");
    if (output == stdout)
      sm_print_fancy_bytelength((long long)to_heap->capacity);
    putc(')', output);
    fprintf(output, "%s", sm_terminal_reset());
    if (evaluating)
      putc('\n', output);

    if (output != stdout) {
      fclose(output); // Close /dev/null if it was opened
    }
  }
}
