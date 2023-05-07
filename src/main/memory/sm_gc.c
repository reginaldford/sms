// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"

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
  char *scan_cursor = (char *)sm_global_current_heap(NULL)->storage;
  while (scan_cursor <
         ((char *)sm_global_current_heap(NULL)->storage) + sm_global_current_heap(NULL)->used) {
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
    case SM_SELF_TYPE: {
      sm_self *self = (sm_self *)current_obj;
      self->context = (sm_cx *)sm_meet_object((sm_object *)self->context);
      break;
    }
    default:
      break;
    }
  }
}

// Copying GC
void sm_garbage_collect() {
  if (sm_global_current_heap(NULL)->used != 0) {
    // Build "to" heap if necessary, same size as current
    if (sm_global_other_heap(NULL) == NULL)
      sm_global_other_heap(sm_new_heap(sm_global_current_heap(NULL)->capacity));

    // Swap heaps now
    sm_heap *other_heap = sm_global_current_heap(sm_global_other_heap(NULL));
    sm_global_other_heap(other_heap);

    // Consider this heap empty now
    sm_global_current_heap(NULL)->used = 0;

    // Reset the space array
    sm_global_space_array(NULL)->size = 0;

    // Copy root (the global context)
    *(sm_global_lex_stack(NULL)->top) =
      (sm_cx *)sm_move_to_new_heap((sm_object *)*(sm_global_lex_stack(NULL)->top));

    // Inflate
    sm_inflate_heap();

    // For tracking purposes
    sm_gc_count(1);
  }
  // This will be a global variable
  printf("\n%i bytes used after gc.\n", sm_global_current_heap(NULL)->used);
}
