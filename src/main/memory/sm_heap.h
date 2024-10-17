// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

/// Stores all of the active objects of SMS
typedef struct sm_heap {
  /// The capacity of this heap in bytes. Copying GC using 2 heaps
  uint32_t capacity;
  /// Number of bytes used in this heap
  uint32_t used;
  /// Pointer to the beginning of the memory heap content
  char *storage;
  // Minimap is 1 bit for every 64 bits of capacity
  // But the ptr is NULL if we have no minimap
  char *map;
} sm_heap;

/// Create a new heap
struct sm_heap *sm_new_heap(uint32_t capacity, bool map);
/// Provide a pointer to a newly allocated space (size specified in bytes) in the main heap.
void *sm_malloc(uint32_t size);
/// Provide a pointer to a newly allocated space (size specified in bytes) in the specified heap.
void *sm_malloc_at(struct sm_heap *heap, uint32_t size);
/// Return whether this pointer aims at the beginning of a registered object of provided heap
bool sm_heap_has_object(sm_heap *heap, void *guess);
/// Provide a pointer to a newly allocated space of the specified size and copy the data specified
/// by ptr to the new space.
void *sm_realloc(void *ptr, uint32_t size);
/// Provide a pointer to a newly allocated space of the specified size and copy the data
/// specified, at the specified heap
void *sm_realloc_at(struct sm_heap *dest, void *ptr, uint32_t size);
/// Zero the heap and map if map exists, set used to 0
void sm_heap_clear(struct sm_heap *h);
/// Returns whether the object is contained in the specified heap.
bool sm_is_within_heap(void *obj, sm_heap *heap);
/// Rounds the number of bytes up to the nearest multiple of 4 bytes
uint32_t sm_round_size(uint32_t size);
/// Rounds the number of bytes up to the nearest multiple of 8 bytes
uint32_t sm_round_size64(uint32_t size);
/// Dump the current memory heap to the specified file
int sm_mem_dump(sm_heap *heap, char *fname);
/// Frees all heaps to prepare for exiting the program
void sm_mem_cleanup();
/// Prints every object in the current heap to stdout
void sm_sprint_dump();
/// Dumps the memory with a counting file name. current_#.mem and  other_#.mem starting with 1
void sm_dump_and_count();
/// Swap the ptrs of these two heaps
void sm_swap_heaps(sm_heap **, sm_heap **);
/// Scan the heap and generate the bitmap. Return true on success.
bool sm_heap_scan(sm_heap *h);
/// Mark this heap's map, if it has one
void sm_heap_register_object(sm_heap *heap, void *guess);
