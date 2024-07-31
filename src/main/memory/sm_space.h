// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

/// An object representing an unused memory space
typedef struct sm_space {
  enum sm_object_type my_type;
  uint16_t            size;
} sm_space;

/** The space tuple is a global tuple of pointers to spaces in the heap.
This tuple is not stored inside the heap.
Has pointers to spaces following the struct.
If an algorithm knowingly generates garbage, it may add spaces to the space tuple, which can then be
checked for new space to use as opposed to actually using more space in the heap.
**/
typedef struct sm_space_tuple {
  uint32_t size;
  uint32_t capacity;
} sm_space_tuple;

/// Results from sm_space_tuple_find and sm_space_tuple_find_by_pos
typedef struct sm_search_result {
  bool     found;
  uint32_t index;
} sm_search_result;

/// Create a new space
sm_space *sm_new_space(void *trash, uint32_t size);
/// Create a new space tuple
sm_space_tuple *sm_new_space_tuple(uint32_t size, uint32_t capacity);
/// Actually relies on OS realloc. This should be extremely rare
sm_space_tuple *sm_resize_space_tuple(sm_space_tuple *table, uint32_t new_capacity);
/// YOU ARE HERE
sm_space       **sm_get_space_tuple(sm_space_tuple *table);
sm_space_tuple  *sm_space_add_by_pos(sm_space *space, sm_space_tuple *table);
sm_space_tuple  *sm_space_add(sm_space *space, sm_space_tuple *table);
sm_search_result sm_space_tuple_find_by_pos(sm_space_tuple *table, sm_space *space);
sm_search_result sm_space_tuple_find(sm_space_tuple *table, uint32_t size);
bool             sm_space_rm_by_pos(sm_space *space);
void             sm_space_rm_by_index(sm_space_tuple *spt, uint32_t index_to_remove);
sm_space        *sm_new_space_after(void *object, uint32_t size);
