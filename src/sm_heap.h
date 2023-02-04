// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

typedef struct sm_heap {
  unsigned int capacity;
  unsigned int used;
  char        *storage;
} sm_heap;

struct sm_heap *sm_new_heap(unsigned int capacity);
void           *sm_malloc(unsigned int size);
void           *sm_realloc(void *ptr, unsigned int size);
bool            sm_is_within_heap(void *obj, sm_heap *heap);
int             sm_round_size(int size);
int             sm_mem_dump(sm_heap *heap, char *fname);
void            sm_mem_cleanup();
