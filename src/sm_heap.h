// This project is licensed under the BSD 2 clause license. See LICENSE.txt for more information.
typedef struct sm_heap {
  unsigned int capacity;
  unsigned int used;
  sm_object   *storage;
} sm_heap;

struct sm_heap *sm_new_heap(unsigned int capacity);
void           *sm_malloc(unsigned int size);
sm_object      *sm_realloc(sm_object *ptr, unsigned int size);
bool            sm_is_within_heap(sm_object *obj, sm_heap *heap);
int             sm_round_size(int size);
int             sm_mem_dump(sm_heap *heap, char *fname);
void            sm_mem_cleanup();
