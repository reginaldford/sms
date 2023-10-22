// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

/// An object representing an unused memory space
typedef struct sm_space {
  enum sm_object_type my_type;
  unsigned short int  size;
} sm_space;

/** The space array is a global array of pointers to spaces in the heap.
This array is not stored inside the heap.
Has pointers to spaces following the struct.
If an algorithm knowingly generates garbage, it may add spaces to the space array, which can then be
checked for new space to use as opposed to actually using more space in the heap.
**/
typedef struct sm_space_array {
  unsigned int size;
  unsigned int capacity;
} sm_space_array;

/// Results from sm_space_array_find and sm_space_array_find_by_pos
typedef struct sm_search_result {
  bool         found;
  unsigned int index;
} sm_search_result;

/// Create a new space
sm_space *sm_new_space(void *trash, unsigned int size);
/// Create a new space array
sm_space_array *sm_new_space_array(unsigned int size, unsigned int capacity);
/// Actually relies on OS realloc. This should be extremely rare
sm_space_array *sm_resize_space_array(sm_space_array *table, unsigned int new_capacity);
/// YOU ARE HERE
sm_space       **sm_get_space_array(sm_space_array *table);
sm_string       *sm_space_to_string(sm_space *self);
sm_space_array  *sm_space_add_by_pos(sm_space *space, sm_space_array *table);
sm_space_array  *sm_space_add(sm_space *space, sm_space_array *table);
sm_search_result sm_space_array_find_by_pos(sm_space_array *table, sm_space *space);
sm_search_result sm_space_array_find(sm_space_array *table, unsigned int size);
bool             sm_space_rm_by_pos(sm_space *space);
void             sm_space_rm_by_index(sm_space_array *spt, unsigned int index_to_remove);
sm_space        *sm_new_space_after(void *object, unsigned int size);
