// The following file is provided under the BSD 2-clause license. For more info, read LICENSE.txt.

#include "sms.h"

sm_object *sm_move_to_new_heap(sm_object *obj) {
  sm_object *new_obj = sm_realloc(obj, sm_sizeof(obj));
  // overwrite the old object. sm_pointer is NOT larger
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
      // Updating the entries
      sm_context_entry *table = sm_context_entries(cx);
      for (unsigned int i = 0; i < ((sm_context *)current_obj)->size; i++) {
        table[i].name  = (sm_string *)sm_meet_object((sm_object *)table[i].name);
        table[i].value = sm_meet_object(table[i].value);
      }
      // Updating the parent
      if (cx->parent != NULL)
        cx->parent = (sm_context *)sm_meet_object((sm_object *)cx->parent);
      // Updating the children
      if (cx->children != NULL) {
        cx->children = (sm_expr *)sm_meet_object((sm_object *)cx->children);
      }
      break;
    }
    case sm_expr_type: {
      sm_expr *expr = (sm_expr *)current_obj;
      if (expr->op == sm_siblings) {
        //!! YOU ARE HERE
        // I need to add this siblings expr to a list
        // after gc is done, place the siblings expressions on the heap
        // inflate_sibilngs will then discard siblings if
        // they dont have a pointer in place, meaning they are garbage.
        for (unsigned int i = 0; i < expr->size; i++) {
          sm_object *new_obj = sm_meet_object(sm_expr_get_arg(expr, i));
          sm_set_expr_arg(expr, i, (sm_object *)new_obj);
        }
        break;
      } else {
        for (unsigned int i = 0; i < expr->size; i++) {
          sm_object *new_obj = sm_meet_object(sm_expr_get_arg(expr, i));
          sm_set_expr_arg(expr, i, (sm_object *)new_obj);
        }
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
    sm_global_other_heap(sm_global_current_heap(sm_global_other_heap(NULL)));

    // consider this heap empty now
    sm_global_current_heap(NULL)->used = 0;

    // reset the space array
    sm_global_space_array(NULL)->size = 0;

    // copy roots
    sm_context *root_cx =
      (sm_context *)sm_move_to_new_heap((sm_object *)*(sm_global_lex_stack(NULL)->top));

    // inflate
    sm_inflate_heap();

    // update global variables
    // sm_global_context((sm_context *)((sm_pointer *)sm_global_context(NULL))->address);
    *(sm_global_lex_stack(NULL)->top) = root_cx;

    // for tracking purposes
    sm_gc_count(1);
  }
  printf("%i bytes used after gc.\n", sm_global_current_heap(NULL)->used);
}
