// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "sms.h"

// Create a new context, an array of key_values sorted by key
sm_context *sm_new_context(unsigned int size, unsigned int capacity, sm_context *parent) {
  sm_context *new_context =
    (sm_context *)sm_malloc(sizeof(sm_context) + sizeof(sm_context_entry) * capacity);
  new_context->my_type  = sm_context_type;
  new_context->parent   = parent;
  new_context->size     = size;
  new_context->capacity = capacity;
  new_context->children = NULL;
  return new_context;
}

// Retreive the tailing array
sm_context_entry *sm_context_entries(sm_context *context) {
  return (sm_context_entry *)&(context[1]);
}

// Mutates the children to add one element
sm_context *sm_context_add_child(sm_context *cx, sm_object *child) {
  sm_expr *array = cx->children;
  if (array == NULL) {
    cx->children = sm_new_expr(sm_siblings, child);
    return cx;
  }
  cx->children = sm_append_to_expr(cx->children, child);
  return cx;
}

// Binary search to find the index with this key
// Else, return the position where it should be added
// For variable reference on lhs
sm_search_result sm_context_find_index(sm_context *context, sm_string *var_string) {
  sm_context_entry *context_entries = sm_context_entries(context);

  if (context->size == 0)
    return (sm_search_result){.found = false, .index = 0};
  char        *var_name    = &(var_string->content);
  unsigned int lower_limit = 0;
  unsigned int upper_limit = context->size == 0 ? 0 : context->size - 1;
  int          comparison  = 1;
  unsigned int guess_point = (upper_limit + lower_limit) / 2.0;
  while (lower_limit < upper_limit && comparison != 0) {
    comparison = strcmp(&(context_entries[guess_point].name->content), var_name);
    if (comparison == 0)
      return (sm_search_result){.found = true, .index = guess_point};
    else if (comparison > 0)
      upper_limit = guess_point == 0 ? 0 : guess_point - 1;
    else
      lower_limit = guess_point + 1;
    guess_point = (upper_limit + lower_limit) / 2.0;
  }
  comparison = strcmp(&(context_entries[guess_point].name->content), var_name);
  if (comparison == 0)
    return (sm_search_result){.found = true, .index = guess_point};
  if (comparison < 0) {
    return (sm_search_result){.found = false, .index = guess_point + 1};
  } else { // comparison > 0
    return (sm_search_result){.found = false, .index = guess_point};
  }
}

// Search for a variable, and traverse the 'parent' pointers to search
// Used for variable reference on rhs
sm_search_result_cascading sm_context_find_far(sm_context *context, sm_string *var_string) {
  sm_search_result sr = sm_context_find_index(context, var_string);
  if (sr.found == true) {
    return (sm_search_result_cascading){.found = true, .index = sr.index, .context = context};
  } else if (context->parent != NULL && context->parent != context) {
    return sm_context_find_far(context->parent, var_string);
  } else {
    return (sm_search_result_cascading){.found = false, .index = 0, .context = context};
  }
}

bool sm_context_update_relatives(sm_context *self, sm_context *old_self) {
  if (self != old_self) {
    if (self->parent != NULL) {
      // updating self among parent's children
      sm_expr *siblings = self->parent->children;
      if (siblings != NULL) {
        for (unsigned int i = 0; i < siblings->size; i++) {
          sm_object *obj = sm_expr_get_arg(siblings, i);
          if (obj == (sm_object *)old_self) {
            sm_set_expr_arg(siblings, i, (sm_object *)self);
            break;
          }
        }
      }
    }
    // updating self as childrens' parent
    sm_expr *siblings = self->children;
    if (siblings != NULL) {
      for (unsigned int i = 0; i < siblings->size; i++) {
        sm_object *obj = sm_expr_get_arg(siblings, i);
        switch (obj->my_type) {
        case sm_context_type: {
          ((sm_context *)obj)->parent = self;
          break;
        }
        case sm_meta_type: {
          ((sm_meta *)obj)->scope = self;
          break;
        }
        case sm_fun_type: {
          ((sm_fun *)obj)->parent = self;
          break;
        }
        }
      }
    }
    if ((sm_context *)*(sm_global_lex_stack(NULL)->top) == old_self) {
      sm_stack_pop(sm_global_lex_stack(NULL));
      sm_stack_push(sm_global_lex_stack(NULL), self);
    }
    return true;
  }
  return false;
}

// If the key exists, mutate the key_value to have the new value
// Else, add a key_value with this key and value
sm_context *sm_context_set(sm_context *context, sm_string *name, void *val) {
  sm_context                *current_context = context;
  sm_search_result_cascading sr              = sm_context_find_far(current_context, name);
  if (sr.found == true) {
    sm_context_entry *context_entries = sm_context_entries(sr.context);
    context_entries[sr.index].value   = val;
    return context;
  }
  sm_search_result sr2 = sm_context_find_index(current_context, name);
  if (current_context->size == current_context->capacity) {
    unsigned int new_capacity =
      ((unsigned int)(current_context->capacity * sm_global_growth_factor(0))) + 1;

    sm_context *new_cx = (sm_context *)sm_realloc(
      (sm_object *)current_context, sizeof(sm_context) + sizeof(sm_context_entry) * new_capacity);
    new_cx->capacity = new_capacity;
    sm_context_update_relatives(new_cx, current_context);
    current_context = new_cx;
  }
  current_context->size += 1;
  sm_context_entry *context_entries = sm_context_entries(current_context);
  for (unsigned int i = current_context->size; i > sr2.index + 1; i--)
    context_entries[i - 1] = context_entries[i - 2];
  context_entries[sr2.index].name  = name;
  context_entries[sr2.index].value = val;
  return current_context;
}

// Remove a key_value identified by the key
bool sm_context_rm(sm_context *self, sm_symbol *sym) {
  sm_search_result sr = sm_context_find_index(self, sym->name);
  if (sr.found == true) {
    sm_context_entry *context_entries = sm_context_entries(self);
    for (unsigned int i = sr.index; i + 1 < self->size; i++) {
      context_entries[i] = context_entries[i + 1];
    }
    self->size -= 1;
    self->capacity = self->size;
    // Putting a space for remaining space
    sm_new_space_after(self, sizeof(sm_context_entry));
    return true;
  } else {
    printf("Could not find variable to remove: %s\n", &(sym->name->content));
    return false;
  }
}

// Print to string buffer a description of this context
// Return the string length
unsigned int sm_context_sprint(sm_context *self, char *buffer, bool fake) {
  if (!fake)
    buffer[0] = '{';
  if (self->size == 0) {
    if (!fake)
      buffer[1] = '}';
    return 2;
  }
  sm_context_entry *ce     = sm_context_entries(self);
  unsigned int      cursor = 1;
  for (unsigned int object_index = 0; object_index + 1 <= self->size; object_index++) {
    cursor += sm_context_entry_sprint(&(ce[object_index]), &(buffer[cursor]), fake);
    if (object_index + 1 != self->size) {
      if (!fake)
        buffer[cursor] = ';';
      cursor++;
    }
  }
  if (!fake)
    buffer[cursor] = '}';
  cursor++;
  return cursor;
}

// Print to string buffer a description of this context entry
unsigned int sm_context_entry_sprint(sm_context_entry *ce, char *buffer, bool fake) {
  if (!fake)
    sm_strncpy(buffer, &(ce->name->content), ce->name->size);
  int cursor = ce->name->size;
  if (!fake)
    buffer[cursor] = '=';
  cursor++;
  cursor += sm_object_sprint((sm_object *)ce->value, &(buffer[cursor]), fake);
  return cursor;
}

// Return the object at a specific index.
// Does not check for validity of index.
sm_object *sm_context_get(sm_context *self, unsigned int index) {
  sm_context_entry *entries = sm_context_entries(self);
  return entries[index].value;
}

// Return the object by name
// Does not search the ancestry for the value
sm_object *sm_context_get_by_name(sm_context *self, sm_string *name) {
  sm_context_entry *entries = sm_context_entries(self);
  sm_search_result  sr      = sm_context_find_index(self, name);
  if (sr.found == true) {
    return entries[sr.index].value;
  }
  return NULL;
}

// Return the object by name
// Searches the ancestry for the value
sm_object *sm_context_get_by_name_far(sm_context *self, sm_string *name) {
  sm_search_result_cascading src = sm_context_find_far(self, name);
  if (src.found == true) {
    sm_context_entry *entries = sm_context_entries(src.context);
    return entries[src.index].value;
  }
  return NULL;
}
