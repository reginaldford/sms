// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"

// Overwrites an existing object to create a space or returns NULL
sm_space *sm_new_space(void *trash, uint32_t size) {
  // Size sets the whole obj size manually
  // Size must be more than or equal to sizeof(sm_space)
  sm_space *new_space = (sm_space *)trash;
  // sm_space needs to be the highest type value
  new_space->my_type = SM_SPACE_TYPE;
  new_space->size    = size;
  sm_global_space_tuple(sm_space_add(new_space, sm_global_space_tuple(NULL)));
  return new_space;
}

// New space just after the given object
sm_space *sm_new_space_after(void *object, uint32_t size) {
  // Size sets the whole obj size manually
  // Size must be more than sizeof(sm_space)
  return sm_new_space(((char *)object) + sm_sizeof(object), size);
}

// create a new tuple designed to hold spaces, sorted by size
sm_space_tuple *sm_new_space_tuple(uint32_t size, uint32_t capacity) {
  sm_space_tuple *new_table = malloc(sizeof(sm_space_tuple) + sizeof(sm_space *) * capacity);
  new_table->size           = size;
  new_table->capacity       = capacity;
  return new_table;
}

// resize the space tuple
sm_space_tuple *sm_resize_space_tuple(sm_space_tuple *table, uint32_t new_capacity) {
  sm_space_tuple *new_table =
    realloc(table, sizeof(sm_space_tuple) + sizeof(sm_space *) * new_capacity);
  new_table->size     = MIN(table->size, new_capacity);
  new_table->capacity = new_capacity;
  return new_table;
}

// get the c tuple. we need to work on terminology here
sm_space **sm_get_space_tuple(sm_space_tuple *table) { return (sm_space **)&(table[1]); }

// find a matching space by size in the size sorted tuple
sm_search_result sm_space_tuple_find(sm_space_tuple *table, uint32_t size) {
  sm_space **table_entries = sm_get_space_tuple(table);

  if (table->size == 0)
    return (sm_search_result){.found = false, .index = 0};

  uint32_t lower_limit = 0;
  uint32_t upper_limit = table->size == 0 ? 0 : table->size - 1;
  int      comparison  = 1;
  uint32_t guess_point = (upper_limit + lower_limit) / 2.0;

  while (lower_limit < upper_limit && comparison != 0) {
    comparison = table_entries[guess_point]->my_type - SM_SPACE_TYPE - size;
    if (comparison == 0)
      return (sm_search_result){.found = true, .index = guess_point};
    else if (comparison > 0)
      upper_limit = guess_point == 0 ? 0 : guess_point - 1;
    else
      lower_limit = guess_point + 1;
    guess_point = (upper_limit + lower_limit) / 2.0;
  }
  comparison = table_entries[guess_point]->my_type - SM_SPACE_TYPE - size;

  if (comparison == 0)
    return (sm_search_result){.found = true, .index = guess_point};
  if (comparison < 0) {
    return (sm_search_result){.found = false, .index = guess_point + 1};
  } else // comparison > 0
  {
    return (sm_search_result){.found = false, .index = guess_point};
  }
}

// add a space to the space tuple, keeping it sorted
sm_space_tuple *sm_space_add(sm_space *space, sm_space_tuple *table) {
  sm_search_result sr            = sm_space_tuple_find(table, space->size);
  sm_space       **table_entries = sm_get_space_tuple(table);

  if (table->size == table->capacity) {
    int new_capacity = ((int)(table->capacity * sm_global_growth_factor(0))) + 1;
    table            = sm_resize_space_tuple(table, new_capacity);
  }

  table->size += 1;
  table_entries = sm_get_space_tuple(table);

  for (uint32_t i = table->size; i > sr.index + 1; i--)
    table_entries[i - 1] = table_entries[i - 2];

  table_entries[sr.index] = space;

  return table;
}

// use sm_space_tuple_find to get a space index
void sm_space_rm_by_index(sm_space_tuple *spt, uint32_t index_to_remove) {
  if (index_to_remove < spt->size) {
    sm_space **ptr_tuple = sm_get_space_tuple(spt);
    spt->size--;
    for (uint32_t i = index_to_remove; i + 2 <= spt->size; i++) {
      ptr_tuple[i] = ptr_tuple[i + 1];
    }
    return;
  }
}
