// This project is licensed under the BSD 2 clause license. See LICENSE.txt for more information.
#include "sms.h"

struct sm_context *sm_new_context(unsigned int capacity) {
  sm_context *new_context =
    (sm_context *)sm_malloc(sizeof(sm_context) + sizeof(sm_context_entry) * capacity);
  new_context->my_type  = sm_context_type;
  new_context->size     = 0;
  new_context->capacity = capacity;
  new_context->parent   = NULL; // for user-made objects
  return new_context;
}

sm_context_entry *sm_context_entries(sm_context *context) {
  return (sm_context_entry *)&(context[1]);
}

sm_string *spaces(int how_many) { return sm_new_string_of(how_many, sm_new_string(1, " ")); }

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
      // guess_point is unsigned, so we need to check against unsigned 0 - 1 here
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
  for (int i = current_context->size - 1; i > sr.index; i--)
    context_entries[i] = context_entries[i - 1];
  context_entries[sr.index].name  = name;
  context_entries[sr.index].value = val;
  return current_context;
}

bool sm_delete(sm_symbol *sym) {
  search_result sr = sm_find_var_index(sm_global_context(NULL), sym->name);
  if (sr.found == true) {
    sm_context       *context         = sm_global_context(NULL);
    sm_context_entry *context_entries = sm_context_entries(context);
    for (unsigned int i = sr.index; i < context->size - 1; i++) {
      context_entries[i] = context_entries[i + 1];
    }
    context->size -= 1;
    context->capacity = context->size;
    // putting a spacer for remaining space
    sm_new_spacer(&(context_entries[context->size]), sizeof(sm_context_entry));
    // TODO: register the spacer here.
    return true;
  } else {
    printf("Could not find variable to delete: %s\n", &(sym->name->content));
    return false;
  }
}

sm_string *sm_context_to_string(sm_context *self) {
  if (self->size == 0) {
    return sm_new_string(3, "{ }");
  }
  sm_context_entry *ce     = sm_context_entries(self);
  sm_string        *answer = sm_new_string(2, "{ ");
  if (self->size > 0) {
    for (unsigned int object_index = 0; object_index <= self->size - 1; object_index++) {
      answer =
        sm_concat_strings_conserving(answer, sm_context_entry_to_string(&(ce[object_index])));
    }
  }
  return sm_concat_strings_conserving(answer, sm_new_string(1, "}"));
}

sm_string *sm_context_entry_to_string(sm_context_entry *ce) {
  sm_string *output_str = ce->name;
  output_str            = sm_concat_strings_conserving(output_str, sm_new_string(3, " = "));
  output_str            = sm_concat_strings_conserving(output_str, sm_object_to_string(ce->value));
  output_str            = sm_concat_strings_conserving(output_str, sm_new_string(2, "; "));
  return output_str;
}
