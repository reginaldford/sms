#include "sms.h"

struct sm_context *sm_new_context(unsigned int capacity) {
  sm_context *new_context =
      (sm_context *)sm_malloc(sizeof(sm_context) + sizeof(sm_table_entry) * capacity);
  new_context->my_type  = sm_context_type;
  new_context->size     = 0;
  new_context->capacity = capacity;
  new_context->parent   = NULL; // for user-made objects
  return new_context;
}

sm_table_entry *get_table_entries(sm_context *context) { return (sm_table_entry *)&(context[1]); }

void sm_print_table(sm_context *context) {
  sm_table_entry *table_entries = get_table_entries(context);
  printf("## SYMBOL TABLE: ##\n");
  fflush(stdout);
  for (int i = 0; i < context->size; i++) {
    sm_object *value = table_entries[i].value;
    char *var_type   = &(sm_object_type_str(value->my_type)->content);
    char *var_name   = &((table_entries[i].name)->content);
    char *desc       = &(sm_object_to_string(value)->content);
    printf("%s : %s : %s\n", var_name, var_type, desc);
  }
}

search_result sm_find_var_index(sm_context *context, sm_string *var_string) {
  sm_table_entry *table_entries = get_table_entries(context);
  if (context->size == 0)
    return (search_result){.found = false, .index = 0};

  char *var_name  = &(var_string->content);
  int lower_limit = 0;
  int upper_limit = context->size - 1;
  int guess_point = (upper_limit + lower_limit) / 2.0;
  int comparison  = 1;

  while (lower_limit < upper_limit && comparison != 0) {
    comparison = strcmp(&(table_entries[guess_point].name->content), var_name);
    if (comparison == 0)
      return (search_result){.found = true, .index = guess_point};
    else if (comparison > 0)
      upper_limit = guess_point - 1;
    else
      lower_limit = guess_point + 1;
    guess_point = (upper_limit + lower_limit) / 2.0;
  }
  comparison = strcmp(&(table_entries[guess_point].name->content), var_name);

  if (comparison == 0)
    return (search_result){.found = true, .index = guess_point};

  else if (comparison < 0) {
    // Special case: element not found and would come after every other element
    if (guess_point == context->size - 1)
      return (search_result){.found = false, .index = context->size};
    else
      return (search_result){.found = false, .index = MIN(guess_point + 1, context->size - 1)};
  } else
    return (search_result){.found = false, .index = MAX(guess_point - 1, 0)};
}

sm_context *sm_set_var(sm_context *context, sm_string *name, void *val) {
  sm_context *current_context   = context;
  sm_table_entry *table_entries = get_table_entries(current_context);
  search_result sr              = sm_find_var_index(current_context, name);

  if (sr.found == true) {
    table_entries[sr.index].value = val;
    return current_context;
  }

  if (current_context->size == current_context->capacity) {
    int new_capacity = ((int)(current_context->capacity * sm_collection_growth_factor(0))) + 1;
    current_context  = (sm_context *)sm_realloc(
         (sm_object *)current_context, sizeof(sm_context) + sizeof(sm_table_entry) * new_capacity);
    current_context->capacity = new_capacity;
  }

  current_context->size += 1;
  table_entries = get_table_entries(current_context);

  for (int i = current_context->size - 1; i > sr.index; i--)
    table_entries[i] = table_entries[i - 1];

  table_entries[sr.index].name  = name;
  table_entries[sr.index].value = val;

  return current_context;
}

bool sm_delete(sm_symbol *sym) {
  search_result sr = sm_find_var_index(sm_global_context(NULL), sym->name);
  if (sr.found == true) {
    sm_context *context           = sm_global_context(NULL);
    sm_table_entry *table_entries = get_table_entries(context);

    for (int i = sr.index; i < context->size - 1; i++) {
      table_entries[i] = table_entries[i + 1];
    }
    context->size -= 1;
    return true;
  } else {
    printf("didnt find that var\n");
    return false;
  }
}

sm_string *sm_context_to_string(sm_context *self) {
  if (self->size == 0) {
    sm_string *answer = sm_new_string(3, "{ }");
    return answer;
  }
  int object_length_sum = 0;
  sm_string *object_strings[self->size];
  sm_table_entry *te = get_table_entries(self);
  for (int object_index = 0; object_index < self->size; object_index++) {
    sm_table_entry *current_object = &te[object_index];
    object_strings[object_index]   = sm_table_entry_to_string(current_object);
    object_length_sum += object_strings[object_index]->size;
  }
  int answer_length = 2 + object_length_sum + self->size * 2 + 1;
  char *buf         = malloc(sizeof(char) * answer_length);
  sprintf(buf, "{ ");
  int buffer_pos = 2;
  for (int object_index = 0; object_index < self->size - 1; object_index++) {
    sprintf(buf + buffer_pos, "%s, ", &(object_strings[object_index]->content));
    //+2 for command and space
    buffer_pos += object_strings[object_index]->size + 2;
  }
  sprintf(buf + buffer_pos, "%s }", &(object_strings[self->size - 1]->content));
  buffer_pos += object_strings[self->size - 1]->size + 2;
  sm_string *answer = sm_new_string(buffer_pos, buf);
  free(buf);
  return answer;
}

sm_string *sm_table_entry_to_string(sm_table_entry *te) {
  char buf[50];
  sprintf(buf, " %s : %s ", &(te->name->content), &(sm_object_to_string(te->value)->content));
  return sm_new_string(strlen(buf), buf);
}
