// The following file is provided under the BSD 2-clause license. For more info, read LICENSE.txt.

#include "sms.h"

// Create a new context, an array of key_values sorted by key
struct sm_context *sm_new_context(unsigned int capacity) {
  sm_context *new_context =
    (sm_context *)sm_malloc(sizeof(sm_context) + sizeof(sm_context_entry) * capacity);
  new_context->my_type  = sm_context_type;
  new_context->size     = 0;
  new_context->capacity = capacity;
  new_context->parent   = NULL; // for user-made objects
  return new_context;
}

// Retreive the tailing array
sm_context_entry *sm_context_entries(sm_context *context) {
  return (sm_context_entry *)&(context[1]);
}

// Binary search to find the index with this key
// Else, return the position where it should be added
search_result sm_find_var_index(sm_context *context, sm_string *var_string) {
  sm_context_entry *context_entries = sm_context_entries(context);

  if (context->size == 0)
    return (search_result){.found = false, .index = 0};
  char        *var_name    = &(var_string->content);
  unsigned int lower_limit = 0;
  unsigned int upper_limit = context->size == 0 ? 0 : context->size - 1;
  int          comparison  = 1;
  unsigned int guess_point = (upper_limit + lower_limit) / 2.0;
  while (lower_limit < upper_limit && comparison != 0) {
    comparison = strcmp(&(context_entries[guess_point].name->content), var_name);
    if (comparison == 0)
      return (search_result){.found = true, .index = guess_point};
    else if (comparison > 0)
      upper_limit = guess_point == 0 ? 0 : guess_point - 1;
    else
      lower_limit = guess_point + 1;
    guess_point = (upper_limit + lower_limit) / 2.0;
  }
  comparison = strcmp(&(context_entries[guess_point].name->content), var_name);

  if (comparison == 0)
    return (search_result){.found = true, .index = guess_point};
  if (comparison < 0) {
    return (search_result){.found = false, .index = guess_point + 1};
  } else { // comparison > 0
    return (search_result){.found = false, .index = guess_point};
  }
}

// If the key exists, mutate the key_value to have the new value.
// Else, add a key_value with this key and value
sm_context *sm_set_var(sm_context *context, sm_string *name, void *val) {
  sm_context       *current_context = context;
  sm_context_entry *context_entries = sm_context_entries(current_context);
  search_result     sr              = sm_find_var_index(current_context, name);
  if (sr.found == true) {
    context_entries[sr.index].value = val;
    return current_context;
  }
  if (current_context->size == current_context->capacity) {
    int new_capacity = ((int)(current_context->capacity * sm_global_growth_factor(0))) + 1;
    current_context  = (sm_context *)sm_realloc(
       (sm_object *)current_context, sizeof(sm_context) + sizeof(sm_context_entry) * new_capacity);
    current_context->capacity = new_capacity;
  }
  current_context->size += 1;
  context_entries = sm_context_entries(current_context);
  for (unsigned int i = current_context->size; i > sr.index + 1; i--)
    context_entries[i - 1] = context_entries[i - 2];
  context_entries[sr.index].name  = name;
  context_entries[sr.index].value = val;
  return current_context;
}

// Remove a key_value identified by the key
bool sm_delete(sm_symbol *sym) {
  search_result sr = sm_find_var_index(sm_global_context(NULL), sym->name);
  if (sr.found == true) {
    sm_context       *context         = sm_global_context(NULL);
    sm_context_entry *context_entries = sm_context_entries(context);
    for (unsigned int i = sr.index; i + 1 < context->size; i++) {
      context_entries[i] = context_entries[i + 1];
    }
    context->size -= 1;
    context->capacity = context->size;
    // putting a space for remaining space
    sm_new_space_after(context, sizeof(sm_context_entry));
    return true;
  } else {
    printf("Could not find variable to delete: %s\n", &(sym->name->content));
    return false;
  }
}

// Return a new string describing this context
sm_string *sm_context_to_string(sm_context *self) {
  if (self->size == 0) {
    return sm_new_string(2, "{}");
  }
  sm_string *new_str = sm_new_string(sm_context_to_string_len((sm_context *)self), "");
  sm_context_sprint(self, (char *)&(new_str[1]));
  (&new_str->content)[new_str->size] = '\0';
  return new_str;
}

unsigned int sm_context_sprint(sm_context *self, char *buffer) {
  buffer[0] = '{';
  if (self->size == 0) {
    buffer[1] = '}';
    return 2;
  }
  sm_context_entry *ce     = sm_context_entries(self);
  unsigned int      cursor = 1;
  for (unsigned int object_index = 0; object_index + 1 <= self->size; object_index++) {
    cursor += sm_context_entry_sprint(&(ce[object_index]), &(buffer[cursor]));
  }
  buffer[cursor++] = '}';
  return cursor;
}

unsigned int sm_context_to_string_len(sm_context *self) {
  if (self->size == 0) {
    return 2;
  }
  sm_context_entry *ce  = sm_context_entries(self);
  unsigned int      sum = 2;
  for (unsigned int object_index = 0; object_index + 1 <= self->size; object_index++) {
    sum += sm_context_entry_to_string_len(&(ce[object_index]));
  }
  return sum;
}

// Return a new string describing this context entry
sm_string *sm_context_entry_to_string(sm_context_entry *ce) {
  sm_string *output_str = sm_string_add_recycle_2nd(ce->name, sm_new_string(1, "="));
  output_str            = sm_string_add(output_str, sm_object_to_string(ce->value));
  return sm_string_add(output_str, sm_new_string(1, ";"));
}

unsigned int sm_context_entry_sprint(sm_context_entry *ce, char *buffer) {
  sm_strncpy(buffer, &(ce->name->content), ce->name->size);
  int cursor       = ce->name->size;
  buffer[cursor++] = '=';
  cursor += sm_object_sprint((sm_object *)ce->value, &(buffer[cursor]));
  buffer[cursor++] = ';';
  return cursor;
}

// Return the length of string that sm_context_entry would produce
unsigned int sm_context_entry_to_string_len(sm_context_entry *ce) {
  unsigned int sum = ce->name->size;
  sum += sm_object_to_string_len(ce->value);
  return sum + 2;
}
