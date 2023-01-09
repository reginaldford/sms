// This project is licensed under the BSD 2 clause license. See LICENSE.txt for more information.

#include "sms.h"

// overwrites an existing object
sm_spacer *sm_new_spacer(void *trash, unsigned int size) {
  // Size must be more than sizeof(sm_spacer)
  // Size sets the whole obj size manually
  sm_spacer *new_spacer = (sm_spacer *)trash;
  new_spacer->my_type   = sm_spacer_type;
  new_spacer->size      = size;
  sm_add_spacer_to_table(new_spacer, sm_global_spacer_table(NULL));
  return new_spacer;
}

sm_spacer_table *sm_new_spacer_table(unsigned int size, unsigned int capacity) {
  sm_spacer_table *new_table = malloc(sizeof(sm_spacer_table) + sizeof(sm_spacer *) * capacity);
  new_table->size            = size;
  new_table->capacity        = capacity;
  return new_table;
}

sm_spacer_table *sm_resize_spacer_table(sm_spacer_table *table, unsigned int new_capacity) {
  sm_spacer_table *new_table =
    realloc(table, sizeof(sm_spacer_table) + sizeof(sm_spacer *) * new_capacity);
  new_table->size     = MIN(table->size, new_capacity);
  new_table->capacity = new_capacity;
  return new_table;
}

sm_spacer **sm_get_spacer_array(sm_spacer_table *table) { return (sm_spacer **)&(table[1]); }

sm_string *sm_spacer_to_string(sm_spacer *self) {
  sm_string *answer = sm_new_string(7, "spacer(");
  sm_string *size   = sm_double_to_string(sm_new_double((double)self->size));
  answer            = sm_concat_strings_conserving(answer, size);
  answer            = sm_concat_strings_conserving(answer, sm_new_string(1, ")"));
  return answer;
}

search_result sm_spacer_table_find(sm_spacer_table *table, sm_spacer *spacer) {
  sm_spacer **table_entries = sm_get_spacer_array(table);
  if (table->size == 0)
    return (search_result){.found = false, .index = 0};
  sm_spacer   *spacer_ptr  = spacer;
  unsigned int lower_limit = 0;
  unsigned int upper_limit = table->size == 0 ? 0 : table->size - 1;
  int          comparison  = 1;
  unsigned int guess_point = (upper_limit + lower_limit) / 2.0;
  while (lower_limit < upper_limit && comparison != 0) {
    comparison = table_entries[guess_point] - spacer_ptr;
    if (comparison == 0)
      return (search_result){.found = true, .index = guess_point};
    else if (comparison > 0)
      // checking against unsigned 0 - 1 at guess_point
      upper_limit = guess_point == 0 ? 0 : guess_point - 1;
    else
      lower_limit = guess_point + 1;
    guess_point = (upper_limit + lower_limit) / 2.0;
  }
  comparison = table_entries[guess_point] - spacer_ptr;

  if (comparison == 0)
    return (search_result){.found = true, .index = guess_point};
  if (comparison < 0) {
    return (search_result){.found = false, .index = guess_point + 1};
  } else { // comparison > 0
    return (search_result){.found = false, .index = guess_point};
  }
}

// TODO: check neighbors and unite spaces.
// TODO: always keep unified representation
// TODO: always keep a size-sorted array also.
// TODO: when we go generational, keep one shared spacer table
// always place objects in the

sm_spacer_table *sm_add_spacer_to_table(sm_spacer *spacer, sm_spacer_table *table) {
  search_result sr            = sm_spacer_table_find(table, spacer);
  sm_spacer   **table_entries = sm_get_spacer_array(table);
  if (sr.found == true) {
    table_entries[sr.index] = spacer;
    return table;
  }
  if (table->size == table->capacity) {
    int new_capacity = ((int)(table->capacity * sm_global_growth_factor(0))) + 1;
    table            = sm_resize_spacer_table(table, new_capacity);
  }
  table->size += 1;
  table_entries = sm_get_spacer_array(table);
  for (unsigned int i = table->size - 1; i > sr.index; i--)
    table_entries[i] = table_entries[i - 1];
  table_entries[sr.index] = spacer;
  return table;
}
