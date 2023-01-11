// This project is licensed under the BSD 2 clause license. See LICENSE.txt for more information.

#include "sms.h"

// overwrites an existing object
sm_space *sm_new_space(void *trash, unsigned int size) {
  // Size sets the whole obj size manually
  // Size must be more than sizeof(sm_space)
  sm_space *new_space = (sm_space *)trash;
  if (size >= sizeof(sm_space)) {
    new_space->my_type = sm_space_type;
    new_space->size    = size;
    sm_global_space_array(sm_add_space(new_space, sm_global_space_array(NULL)));
    return new_space;
  } else
    return NULL;
}

sm_space_array *sm_new_space_array(unsigned int size, unsigned int capacity) {
  sm_space_array *new_table = malloc(sizeof(sm_space_array) + sizeof(sm_space *) * capacity);
  new_table->size           = size;
  new_table->capacity       = capacity;
  return new_table;
}

sm_space_array *sm_resize_space_array(sm_space_array *table, unsigned int new_capacity) {
  sm_space_array *new_table =
    realloc(table, sizeof(sm_space_array) + sizeof(sm_space *) * new_capacity);
  new_table->size     = MIN(table->size, new_capacity);
  new_table->capacity = new_capacity;
  return new_table;
}

sm_space **sm_get_space_array(sm_space_array *table) { return (sm_space **)&(table[1]); }

sm_string *sm_space_to_string(sm_space *self) {
  sm_string *answer = sm_new_string(7, "space(");
  sm_string *size   = sm_double_to_string(sm_new_double((double)self->size));
  answer            = sm_concat_strings_recycle(answer, size);
  answer            = sm_concat_strings_recycle(answer, sm_new_string(1, ")"));
  return answer;
}

search_result sm_space_array_find(sm_space_array *table, unsigned int size) {
  sm_space **table_entries = sm_get_space_array(table);

  if (table->size == 0)
    return (search_result){.found = false, .index = 0};

  unsigned int lower_limit = 0;
  unsigned int upper_limit = table->size == 0 ? 0 : table->size - 1;
  int          comparison  = 1;
  unsigned int guess_point = (upper_limit + lower_limit) / 2.0;

  while (lower_limit < upper_limit && comparison != 0) {
    comparison = table_entries[guess_point]->size - size;
    if (comparison == 0)
      return (search_result){.found = true, .index = guess_point};
    else if (comparison > 0)
      // checking against unsigned 0 - 1 at guess_point
      upper_limit = guess_point == 0 ? 0 : guess_point - 1;
    else
      lower_limit = guess_point + 1;
    guess_point = (upper_limit + lower_limit) / 2.0;
  }
  comparison = table_entries[guess_point]->size - size;

  if (comparison == 0)
    return (search_result){.found = true, .index = guess_point};
  if (comparison < 0) {
    return (search_result){.found = false, .index = guess_point + 1};
  } else { // comparison > 0
    return (search_result){.found = false, .index = guess_point};
  }
}

sm_space_array *sm_add_space(sm_space *space, sm_space_array *table) {
  search_result sr            = sm_space_array_find(table, space->size);
  sm_space    **table_entries = sm_get_space_array(table);

  if (table->size == table->capacity) {
    int new_capacity = ((int)(table->capacity * sm_global_growth_factor(0))) + 1;
    table            = sm_resize_space_array(table, new_capacity);
  }

  table->size += 1;
  table_entries = sm_get_space_array(table);

  for (unsigned int i = table->size; i > sr.index + 1; i--)
    table_entries[i - 1] = table_entries[i - 2];

  table_entries[sr.index] = space;

  return table;
}

void sm_delete_space_by_index(sm_space_array *spt, unsigned int index_to_delete) {
  sm_space **ptr_array = sm_get_space_array(spt);
  for (unsigned int i = index_to_delete; i + 2 < spt->size; i++) {
    ptr_array[i] = ptr_array[i + 1];
  }
  if (index_to_delete + 1 <= spt->size)
    spt->size = spt->size == 0 ? 0 : spt->size - 1;
}
