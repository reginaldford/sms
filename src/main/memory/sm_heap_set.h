// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

/// Stores a set of pointers to heaps.
typedef struct sm_heap_set {
  uint64_t  capacity;
  uint64_t  size;
  sm_heap **heaps;
} sm_heap_set;

/// Add a heap to the set , which might result in reallocation
/// Functional design returns the updated heap.
sm_heap_set *sm_new_heap_set(uint64_t capacity, uint64_t size, sm_heap **heaps);
/// Removes a member of the heap set
bool sm_heap_set_rm(sm_heap *goner, sm_heap_set *hs);
bool sm_heap_set_free(sm_heap_set *hs);
