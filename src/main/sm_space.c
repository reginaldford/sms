// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "sms.h"

// Overwrites an existing object to create a space or returns NULL
sm_space *sm_new_space(void *trash, unsigned int size) {
  // Size sets the whole obj size manually
  // Size must be more than sizeof(sm_space)
  if (size >= sizeof(sm_space)) {
    sm_space *new_space = (sm_space *)trash;
    // sm_space needs to be the highest type value
    new_space->my_type = sm_space_type + size;
    sm_global_space_array(sm_space_add(new_space, sm_global_space_array(NULL)));
    return new_space;
  } else
    return NULL;
}

// New space just after the given object
sm_space *sm_new_space_after(void *object, unsigned int size) {
  // Size sets the whole obj size manually
  // Size must be more than sizeof(sm_space)
  return sm_new_space(((char *)object) + sm_sizeof(object), size);
}

// create a new array designed to hold spaces, sorted by size
sm_space_array *sm_new_space_array(unsigned int size, unsigned int capacity) {
  sm_space_array *new_table = malloc(sizeof(sm_space_array) + sizeof(sm_space *) * capacity);
  new_table->size           = size;
  new_table->capacity       = capacity;
  return new_table;
}

// resize the space array
sm_space_array *sm_resize_space_array(sm_space_array *table, unsigned int new_capacity) {
  sm_space_array *new_table =
    realloc(table, sizeof(sm_space_array) + sizeof(sm_space *) * new_capacity);
  new_table->size     = MIN(table->size, new_capacity);
  new_table->capacity = new_capacity;
  return new_table;
}

// get the c array. we need to work on terminology here
sm_space **sm_get_space_array(sm_space_array *table) { return (sm_space **)&(table[1]); }

sm_string *sm_space_to_string(sm_space *self) {
  sm_string *answer = sm_new_string(7, "space(");
  sm_string *size   = sm_double_to_string(sm_new_double((double)self->my_type - sm_space_type));
  answer            = sm_string_add_recycle(answer, size);
  answer            = sm_string_add_recycle(answer, sm_new_string(1, ")"));
  return answer;
}

// find a matching space by size in the size sorted array
sm_search_result sm_space_array_find(sm_space_array *table, unsigned int size) {
  sm_space **table_entries = sm_get_space_array(table);

  if (table->size == 0)
    return (sm_search_result){.found = false, .index = 0};

  unsigned int lower_limit = 0;
  unsigned int upper_limit = table->size == 0 ? 0 : table->size - 1;
  int          comparison  = 1;
  unsigned int guess_point = (upper_limit + lower_limit) / 2.0;

  while (lower_limit < upper_limit && comparison != 0) {
    comparison = table_entries[guess_point]->my_type - sm_space_type - size;
    if (comparison == 0)
      return (sm_search_result){.found = true, .index = guess_point};
    else if (comparison > 0)
      upper_limit = guess_point == 0 ? 0 : guess_point - 1;
    else
      lower_limit = guess_point + 1;
    guess_point = (upper_limit + lower_limit) / 2.0;
  }
  comparison = table_entries[guess_point]->my_type - sm_space_type - size;

  if (comparison == 0)
    return (sm_search_result){.found = true, .index = guess_point};
  if (comparison < 0) {
    return (sm_search_result){.found = false, .index = guess_point + 1};
  } else { // comparison > 0
    return (sm_search_result){.found = false, .index = guess_point};
  }
}

// add a space to the space array, keeping it sorted
sm_space_array *sm_space_add(sm_space *space, sm_space_array *table) {
  sm_search_result sr            = sm_space_array_find(table, space->my_type - sm_space_type);
  sm_space       **table_entries = sm_get_space_array(table);

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

// use sm_space_array_find to get a space index
void sm_space_rm_by_index(sm_space_array *spt, unsigned int index_to_remove) {
  if (index_to_remove < spt->size) {
    sm_space **ptr_array = sm_get_space_array(spt);
    spt->size--;
    for (unsigned int i = index_to_remove; i + 2 <= spt->size; i++) {
      ptr_array[i] = ptr_array[i + 1];
    }
    return;
  }
}
