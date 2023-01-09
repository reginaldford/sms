// This project is licensed under the BSD 2 clause license. See LICENSE.txt for more information.

typedef struct sm_spacer {
  enum sm_object_type my_type;
  unsigned int        size;
} sm_spacer;

// Not part of the heap
// Has pointers to spacers following the struct
typedef struct sm_spacer_table {
  unsigned int size;
  unsigned int capacity;
} sm_spacer_table;


sm_spacer       *sm_new_spacer(void *trash, unsigned int size);
sm_spacer_table *sm_new_spacer_table(unsigned int size, unsigned int capacity);
sm_spacer_table *sm_resize_spacer_table(sm_spacer_table *table, unsigned int new_capacity);
sm_spacer      **sm_get_spacer_array(sm_spacer_table *table);
sm_string       *sm_spacer_to_string(sm_spacer *self);
sm_spacer_table *sm_add_spacer_to_table(sm_spacer *spacer, sm_spacer_table *table);
