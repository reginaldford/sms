typedef struct sm_memory_heap {
  unsigned int capacity;
  unsigned int used;
  sm_object   *storage;
} sm_memory_heap;
struct sm_memory_heap *sm_new_memory_heap(unsigned int capacity);
struct sm_memory_heap *sm_global_current_heap();
void                  *sm_malloc(unsigned int size);
sm_object             *sm_realloc(sm_object *ptr, unsigned int size);
bool                   sm_is_within_heap(sm_object *obj, sm_memory_heap *heap);
int                    sm_round_size(int size);
int                    sm_mem_dump(sm_memory_heap *heap, char *fname);
void                   sm_mem_cleanup();
