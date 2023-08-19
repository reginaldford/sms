// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"

// Copy the object
sm_object *sm_copy(sm_object *obj) {
  sm_object *new_obj = sm_realloc(obj, sm_sizeof(obj));
  return new_obj;
}

// Deep Copy the object
sm_object *sm_deep_copy(sm_object *obj) {
  if (obj->my_type == SM_EXPR_TYPE) {
    sm_expr *expr = (sm_expr *)sm_copy(obj);
    for (unsigned int i = 0; i < expr->size; i++) {
      sm_expr_set_arg(expr, i, sm_deep_copy(sm_expr_get_arg(expr, i)));
    }
    return obj;
  } else {
    sm_object *new_obj = sm_realloc(obj, sm_sizeof(obj));
    return new_obj;
  }
}

// Copy the object to the new heap
// Leave an sm_pointer in the old space
sm_object *sm_move_to_new_heap(sm_object *obj) {
  sm_object *new_obj = sm_realloc(obj, sm_sizeof(obj));
  // Overwrite the old object. sm_pointer is NOT larger
  sm_new_pointer(obj, new_obj);
  return new_obj;
}

// If obj is an sm_pointer, the object was already moved to the new heap
// Else, copy the object to the new heap and leave an sm_pointer
sm_object *sm_meet_object(sm_object *obj) {
  unsigned short int obj_type = obj->my_type;
  if (obj_type == SM_POINTER_TYPE)
    return ((sm_pointer *)obj)->address;
  else
    return sm_move_to_new_heap(obj);
}

// Copy the objects referenced by the current_obj into the new heap
// and copy all referenced objects until all possible references are copied
void sm_inflate_heap() {
  // Inflate new space. 'meet' every ptr
  // Meeting will copy to new heap if necessary
  char *scan_cursor = (char *)sms_heap->storage;
  while (scan_cursor < ((char *)sms_heap->storage) + sms_heap->used) {
    sm_object *current_obj = (sm_object *)scan_cursor;
    // scan_cursor is not referred to for the rest of the loop
    scan_cursor += sm_sizeof(current_obj);
    switch (current_obj->my_type) {
    case SM_LINK_TYPE: {
      struct sm_link *link = (struct sm_link *)current_obj;
      // Meet the value
      link->value = sm_meet_object((sm_object *)link->value);
      // Meet the next
      if (link->next != NULL)
        link->next = (struct sm_link *)sm_meet_object((sm_object *)link->next);
      break;
    }
    case SM_CX_TYPE: {
      sm_cx *cx = (sm_cx *)current_obj;
      // Meet the parent
      if (cx->parent != NULL)
        cx->parent = (sm_cx *)sm_meet_object((sm_object *)cx->parent);
      // Meet the top cx node
      if (cx->content != NULL)
        cx->content = (sm_node *)sm_meet_object((sm_object *)cx->content);
      break;
    }
    case SM_NODE_TYPE: {
      sm_node *node = (sm_node *)current_obj;
      // Meet the value
      if (node->value != NULL)
        node->value = sm_meet_object((sm_object *)node->value);
      // Meet the next node
      if (node->next != NULL)
        node->next = (struct sm_node *)sm_meet_object((sm_object *)node->next);
      // Meet the children
      if (node->children != NULL)
        node->children = (struct sm_node *)sm_meet_object((sm_object *)node->children);
      break;
    }
    case SM_EXPR_TYPE: {
      sm_expr *expr = (sm_expr *)current_obj;
      for (unsigned int i = 0; i < expr->size; i++) {
        sm_object *new_obj = sm_meet_object(sm_expr_get_arg(expr, i));
        sm_expr_set_arg(expr, i, (sm_object *)new_obj);
      }
      break;
    }
    case SM_SYMBOL_TYPE: {
      sm_symbol *sym = (sm_symbol *)current_obj;
      sym->name      = (sm_string *)sm_meet_object((sm_object *)sym->name);
      break;
    }
    case SM_META_TYPE: {
      sm_meta *meta = (sm_meta *)current_obj;
      meta->address = (sm_object *)sm_meet_object(meta->address);
      break;
    }
    case SM_FUN_TYPE: {
      sm_fun *fun  = (sm_fun *)current_obj;
      fun->content = sm_meet_object((sm_object *)fun->content);
      if (fun->parent != NULL)
        fun->parent = (sm_cx *)sm_meet_object((sm_object *)fun->parent);
      for (unsigned short int i = 0; i < fun->num_params; i++) {
        sm_fun_param *param = sm_fun_get_param(fun, i);
        param->name         = (sm_string *)sm_meet_object((sm_object *)param->name);
        if (param->default_val != NULL) {
          param->default_val = sm_meet_object(param->default_val);
        }
      }
      break;
    }
    case SM_FUN_PARAM_TYPE: {
      sm_fun_param_obj *param = (sm_fun_param_obj *)current_obj;
      param->name             = (sm_string *)sm_meet_object((sm_object *)param->name);
      if (param->default_val != NULL) {
        param->default_val = sm_meet_object((sm_object *)param->default_val);
      }
      break;
    }
    case SM_LOCAL_TYPE: {
      sm_local *local = (sm_local *)current_obj;
      local->name     = (sm_string *)sm_meet_object((sm_object *)local->name);
      break;
    }
    case SM_RETURN_TYPE: {
      sm_return *return_obj = (sm_return *)current_obj;
      return_obj->address   = sm_meet_object((sm_object *)return_obj->address);
      break;
    }
    default:
      break;
    }
  }
}

// Copying GC
void sm_garbage_collect() {
  if (sms_heap->used != 0) {
    // Build "to" heap if necessary, same size as current
    if (sms_other_heap == NULL)
      sms_other_heap = sm_new_heap(sms_heap->capacity);

    // Swap heaps now
    sm_heap *temp  = sms_other_heap;
    sms_other_heap = sms_heap;
    sms_heap       = temp;

    // Consider this heap empty now
    sms_heap->used = 0;

    // Reset the space array
    sm_global_space_array(NULL)->size = 0;

    // Copy root (global context)
    *sm_global_lex_stack(NULL)->top =
      (sm_cx *)sm_move_to_new_heap((sm_object *)*sm_global_lex_stack(NULL)->top);

    // Copy root (true and false singletons)
    sms_true  = (sm_symbol *)sm_meet_object((sm_object *)sms_true);
    sms_false = (sm_symbol *)sm_meet_object((sm_object *)sms_false);

    // Parser output is a root
    if (sm_global_parser_output(NULL))
      sm_global_parser_output(sm_meet_object(((sm_object *)sm_global_parser_output(NULL))));

    // Inflate
    sm_inflate_heap();

    // For tracking purposes
    sm_gc_count(1);
  }
  // This will be a global variable
  if (sm_global_environment(NULL) && sm_global_environment(NULL)->quiet_mode == false) {
    const unsigned int KB = 1024;
    putc('\n', stdout);
    printf("%s", sm_terminal_fg_color(SM_TERM_B_BLACK));
    putc('(', stdout);
    if (sms_heap->used < KB)
      printf("%.3gB", (double)sms_heap->used);
    else if (sms_heap->used < KB * KB)
      printf("%.3gKB", (double)sms_heap->used / KB);
    else if (sms_heap->used < (KB * KB * KB))
      printf("%.3gMB", (double)sms_heap->used / (KB * KB));
    else if (sms_heap->used >= (KB * KB * KB * KB) && sms_heap->used < (KB * KB))
      printf("%.3gGB", (double)sms_heap->used / (KB * KB * KB));
    else if (sms_heap->used >= (KB * KB * KB * KB * KB))
      printf("%.3gTB", (double)sms_heap->used / (KB * KB * KB * KB));
    printf(" / ");
    if (sms_heap->capacity < KB)
      printf("%.3gB", (double)sms_heap->capacity);
    else if (sms_heap->capacity < KB * KB)
      printf("%.3gKB", (double)sms_heap->capacity / KB);
    else if (sms_heap->capacity < (KB * KB * KB))
      printf("%.3gMB", (double)sms_heap->capacity / (KB * KB));
    else if (sms_heap->capacity >= (KB * KB * KB * KB) && sms_heap->capacity < (KB * KB))
      printf("%.3gGB", (double)sms_heap->capacity / (KB * KB * KB));
    else if (sms_heap->capacity >= (KB * KB * KB * KB * KB))
      printf("%.3gTB", (double)sms_heap->capacity / (KB * KB * KB * KB));
    putc(')', stdout);
    printf("%s", sm_terminal_reset());
  }
}
