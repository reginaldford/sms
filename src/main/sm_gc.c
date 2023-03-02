// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "sms.h"

// Copy the object to the new heap
// Leave an sm_pointer in the old space
sm_object *sm_move_to_new_heap(sm_object *obj) {
  sm_object *new_obj = sm_realloc(obj, sm_sizeof(obj));
  // Overwrite the old object. sm_pointer is NOT larger
  sm_new_pointer(obj, new_obj);
  return new_obj;
}

// If obj is an sm_pointer, the object was alreay moved to the new heap
// Else, copy the object to the new heap and leave an sm_pointer
sm_object *sm_meet_object(sm_object *obj) {
  unsigned short int the_expr = obj->my_type;
  if (the_expr == sm_pointer_type)
    return ((sm_pointer *)obj)->address;
  else
    return sm_move_to_new_heap(obj);
}

// If a sibling ptr aims at an sm_pointer, then it was already copied to new space by GC
// Therefore, it's not garbage, and we will keep it
sm_expr *inflate_siblings(sm_expr *siblings) {
  unsigned int num_keepers = 0;
  sm_object   *keepers[siblings->size];
  for (unsigned int i = 0; i < siblings->size; i++) {
    sm_object *current_sibling = sm_expr_get_arg(siblings, i);
    if (current_sibling->my_type == sm_pointer_type) {
      keepers[num_keepers++] = ((sm_pointer *)current_sibling)->address;
    }
  }
  if (num_keepers > 0) {
    sm_expr *new_expr = sm_new_expr_n(sm_siblings_expr, num_keepers, num_keepers);
    for (unsigned int i = 0; i < num_keepers; i++) {
      sm_expr_set_arg(new_expr, i, keepers[i]);
    }
    return new_expr;
  }
  return NULL;
}

// GC for the internal parent/child references in contexts
void inflate_all_siblings() {
  sm_expr *arr = sm_global_parents(NULL);
  for (unsigned int i = 0; i < arr->size; i++) {
    sm_context *parent = (sm_context *)sm_expr_get_arg(arr, i);
    parent->children   = inflate_siblings(parent->children);
  }
  arr->size = 0;
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
    case sm_context_type: {
      sm_context *cx = (sm_context *)current_obj;
      // Meet the entries
      sm_context_entry *table = sm_context_entries(cx);
      for (unsigned int i = 0; i < ((sm_context *)current_obj)->size; i++) {
        table[i].name  = (sm_string *)sm_meet_object((sm_object *)table[i].name);
        table[i].value = sm_meet_object(table[i].value);
      }
      // Meet the parent
      if (cx->parent != NULL)
        cx->parent = (sm_context *)sm_meet_object((sm_object *)cx->parent);
      // Save the siblings list for later processing
      if (cx->children != NULL) {
        sm_expr *e = sm_expr_append(sm_global_parents(NULL), (sm_object *)cx);
        sm_global_parents(e);
      }
      break;
    }
    case sm_expr_type: {
      sm_expr *expr = (sm_expr *)current_obj;
      for (unsigned int i = 0; i < expr->size; i++) {
        sm_object *new_obj = sm_meet_object(sm_expr_get_arg(expr, i));
        sm_expr_set_arg(expr, i, (sm_object *)new_obj);
      }
      break;
    }
    case sm_symbol_type: {
      sm_symbol *sym = (sm_symbol *)current_obj;
      sym->name      = (sm_string *)sm_meet_object((sm_object *)sym->name);
      break;
    }
    case sm_meta_type: {
      sm_meta *meta = (sm_meta *)current_obj;
      meta->address = (sm_object *)sm_meet_object(meta->address);
      break;
    }
    case sm_fun_type: {
      sm_fun *fun  = (sm_fun *)current_obj;
      fun->content = sm_meet_object((sm_object *)fun->content);
      if (fun->parent != NULL)
        fun->parent = (sm_context *)sm_meet_object((sm_object *)fun->parent);
      for (unsigned short int i = 0; i < fun->num_params; i++) {
        sm_fun_param *param = sm_fun_get_param(fun, i);
        param->name         = (sm_string *)sm_meet_object((sm_object *)param->name);
        if (param->default_val != NULL) {
          param->default_val = sm_meet_object(param->default_val);
        }
      }
      break;
    }
    case sm_fun_param_type: {
      sm_fun_param_obj *param = (sm_fun_param_obj *)current_obj;
      param->name             = (sm_string *)sm_meet_object((sm_object *)param->name);
      if (param->default_val != NULL) {
        param->default_val = sm_meet_object((sm_object *)param->default_val);
      }
      break;
    }
    case sm_local_type: {
      sm_local *local = (sm_local *)current_obj;
      local->name     = (sm_string *)sm_meet_object((sm_object *)local->name);
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
      (sm_context *)sm_move_to_new_heap((sm_object *)*(sm_global_lex_stack(NULL)->top));

    // Inflate
    sm_inflate_heap();

    // Inflate context siblings arrays last
    inflate_all_siblings();

    // For tracking purposes
    sm_gc_count(1);
  }
  printf("%i bytes used after gc.\n", sm_global_current_heap(NULL)->used);
}
