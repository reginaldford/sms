// The following file is provided under the BSD 2-clause license. For more info, read LICENSE.txt.

// Anything with a type too high is a space
// sm_space_type is to be subtracted from my_type to calculate the size
typedef struct sm_space {
  enum sm_object_type my_type;
  enum sm_object_type my_type2;
  enum sm_object_type my_type3;
  enum sm_object_type my_type4;
} sm_space;


// Not part of the heap
// Has pointers to spaces following the struct
typedef struct sm_space_array {
  unsigned int size;
  unsigned int capacity;
} sm_space_array;

sm_space       *sm_new_space(void *trash, unsigned int size);
sm_space_array *sm_new_space_array(unsigned int size, unsigned int capacity);
sm_space_array *sm_resize_space_array(sm_space_array *table, unsigned int new_capacity);
sm_space      **sm_get_space_array(sm_space_array *table);
sm_string      *sm_space_to_string(sm_space *self);
sm_space_array *sm_space_add_by_pos(sm_space *space, sm_space_array *table);
sm_space_array *sm_space_add(sm_space *space, sm_space_array *table);
search_result   sm_space_array_find_by_pos(sm_space_array *table, sm_space *space);
search_result   sm_space_array_find(sm_space_array *table, unsigned int size);
bool            sm_delete_space_by_pos(sm_space *space);
void            sm_delete_space_by_index(sm_space_array *spt, unsigned int index_to_delete);
sm_space       *sm_new_space_after(void *object, unsigned int size);
