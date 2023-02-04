// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "sms.h"

sm_object *sm_move_to_new_heap(sm_object *obj) {
  sm_object *new_obj = sm_realloc(obj, sm_sizeof(obj));
  // Overwrite the old object. sm_pointer is NOT larger
  sm_new_pointer(obj, new_obj);
  return new_obj;
}

sm_object *sm_meet_object(sm_object *obj) {
  unsigned short int the_type = obj->my_type;
  if (the_type == sm_pointer_type)
    return ((sm_pointer *)obj)->address;
  else
    return sm_move_to_new_heap(obj);
}

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
    sm_expr *new_expr = sm_new_expr_n(sm_siblings, num_keepers, num_keepers);
    for (unsigned int i = 0; i < num_keepers; i++) {
      sm_set_expr_arg(new_expr, i, keepers[i]);
    }
    return new_expr;
  }
  return NULL;
}

void inflate_all_siblings() {
  sm_expr *arr = sm_global_parents(NULL);
  for (unsigned int i = 0; i < arr->size; i++) {
    sm_context *parent = (sm_context *)sm_expr_get_arg(arr, i);
    parent->children   = inflate_siblings(parent->children);
  }
  arr->size = 0;
}

void sm_inflate_heap() {
  // inflate new space. 'meet' every ptr,
  // meeting will copy to new heap if necessary
  char *scan_cursor = (char *)sm_global_current_heap(NULL)->storage;
  while (scan_cursor <
         ((char *)sm_global_current_heap(NULL)->storage) + sm_global_current_heap(NULL)->used) {
    sm_object *current_obj = (sm_object *)scan_cursor;
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
        // TODO: use custom allocators , this collection is not in the heap
        sm_expr *e = sm_append_to_expr(sm_global_parents(NULL), (sm_object *)cx);
        sm_global_parents(e);
      }
      break;
    }
    case sm_expr_type: {
      sm_expr *expr = (sm_expr *)current_obj;
      for (unsigned int i = 0; i < expr->size; i++) {
        sm_object *new_obj = sm_meet_object(sm_expr_get_arg(expr, i));
        sm_set_expr_arg(expr, i, (sm_object *)new_obj);
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
    default:
      break;
    }

    if (current_obj->my_type <= sm_space_type)
      scan_cursor += sm_sizeof(current_obj);
    else {
      printf("Error: Ending inflation on unrecognized object type: %i\n", current_obj->my_type);
      return;
    }
  }
}

void sm_garbage_collect() {
  if (sm_global_current_heap(NULL)->used != 0) {
    // build "to" heap if necessary, same size as current
    if (sm_global_other_heap(NULL) == NULL)
      sm_global_other_heap(sm_new_heap(sm_global_current_heap(NULL)->capacity));

    // swap heaps now
    sm_heap *other_heap = sm_global_current_heap(sm_global_other_heap(NULL));
    sm_global_other_heap(other_heap);

    // consider this heap empty now
    sm_global_current_heap(NULL)->used = 0;

    // reset the space array
    sm_global_space_array(NULL)->size = 0;

    // copy root (the global context)
    *(sm_global_lex_stack(NULL)->top) =
      (sm_context *)sm_move_to_new_heap((sm_object *)*(sm_global_lex_stack(NULL)->top));

    // inflate
    sm_inflate_heap();

    // inflate context siblings arrays last
    inflate_all_siblings();

    // for tracking purposes
    sm_gc_count(1);
  }
  printf("%i bytes used after gc.\n", sm_global_current_heap(NULL)->used);
}
