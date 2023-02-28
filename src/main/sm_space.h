// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

// Anything with a type too high is a space
// sm_space_expr is to be subtracted from my_type to calculate the size
typedef struct sm_space {
  enum sm_object_expr my_type;
  unsigned short int  size;
} sm_space;


// Not part of the heap
// Has pointers to spaces following the struct
typedef struct sm_space_array {
  unsigned int size;
  unsigned int capacity;
} sm_space_array;

sm_space        *sm_new_space(void *trash, unsigned int size);
sm_space_array  *sm_new_space_array(unsigned int size, unsigned int capacity);
sm_space_array  *sm_resize_space_array(sm_space_array *table, unsigned int new_capacity);
sm_space       **sm_get_space_array(sm_space_array *table);
sm_string       *sm_space_to_string(sm_space *self);
sm_space_array  *sm_space_add_by_pos(sm_space *space, sm_space_array *table);
sm_space_array  *sm_space_add(sm_space *space, sm_space_array *table);
sm_search_result sm_space_array_find_by_pos(sm_space_array *table, sm_space *space);
sm_search_result sm_space_array_find(sm_space_array *table, unsigned int size);
bool             sm_space_rm_by_pos(sm_space *space);
void             sm_space_rm_by_index(sm_space_array *spt, unsigned int index_to_remove);
sm_space        *sm_new_space_after(void *object, unsigned int size);
