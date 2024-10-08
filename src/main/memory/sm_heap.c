// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"

extern sm_heap     *sms_heap;
extern sm_heap     *sms_other_heap;
extern sm_symbol   *sms_true;
extern sm_symbol   *sms_false;
extern sm_heap     *sms_symbol_heap;
extern sm_heap     *sms_symbol_name_heap;
extern sm_heap_set *sms_all_heaps;
extern void        *memory_marker1;
extern void        *memory_marker2;


// For rounding up object size to the next multiple of 4 bytes.
uint32_t sm_round_size(uint32_t size) { return ((size) + 3) & ~3; }
// For rounding up object size to the next multiple of 8 bytes.
uint32_t sm_round_size64(uint32_t size) { return ((size) + 7) & ~7; }

// Create a new heap of some capacity
sm_heap *sm_new_heap(uint32_t capacity) {
  sm_heap *new_heap  = (sm_heap *)malloc(sizeof(sm_heap) + capacity);
  new_heap->capacity = capacity;
  new_heap->used     = 0;
  new_heap->storage  = (char *)(new_heap + 1);
  sm_heap_set_add(sms_all_heaps, new_heap);
  return new_heap;
}

// Internal 'malloc'
void *sm_malloc(uint32_t size) {
  uint32_t bytes_used      = sms_heap->used;
  uint32_t next_bytes_used = sms_heap->used + size;
  if (next_bytes_used > sms_heap->capacity) {
    // Cleanup
    if (!sms_other_heap)
      sms_other_heap = sm_new_heap(sms_heap->capacity);
    memory_marker2 = __builtin_frame_address(0);
    sm_garbage_collect(sms_heap, sms_other_heap);
    // Empty this heap and Swap heaps
    sm_swap_heaps(&sms_heap, &sms_other_heap);
    sms_other_heap->used = 0;
    next_bytes_used      = sms_heap->used + size;
    if (next_bytes_used > sms_heap->capacity) {
      fprintf(stderr, "Exhausted heap memory.\n");
      exit(1);
    }
  }
  sms_heap->used = next_bytes_used;
  return (void *)(((char *)sms_heap->storage) + bytes_used);
}

// Internal 'malloc' with alternative heap
void *sm_malloc_at(struct sm_heap *heap, uint32_t size) {
  if (heap == sms_heap)
    return sm_malloc(size);
  uint32_t bytes_used = heap->used;
  heap->used += size;
  return (void *)(((char *)heap->storage) + bytes_used);
}

// Reallocate memory space for resizing or recreating objects
void *sm_realloc(void *obj, uint32_t size) {
  sm_object *new_space = sm_malloc(size);
  return memcpy(new_space, obj, sm_sizeof(obj));
}

// Reallocate memory space for resizing or recreating objects, at specified heap
void *sm_realloc_at(struct sm_heap *dest, void *obj, uint32_t size) {
  sm_object *new_space = sm_malloc_at(dest, size);
  return memcpy(new_space, obj, size);
}

// Is the object within this heap?
bool sm_is_within_heap(void *obj, sm_heap *heap) {
  return (obj >= (void *)heap) && (obj < (void *)((char *)heap) + heap->used);
}

// For advanced debugging, run this at any point and examine the heap snapshot as a file
int sm_mem_dump(sm_heap *heap, char *name) {
  void *buffer        = heap->storage;
  int   buffer_length = heap->used;
  // Open a file for writing
  FILE *fp = fopen(name, "wb");
  // Write the data to the file
  fwrite(buffer, 1, buffer_length, fp);
  fflush(fp);
  // Close the file
  // Returns 0 if there are no problems
  return fclose(fp);
}

// Take multiple snapshots of both heaps for inflation debugging
void sm_dump_and_count() {
  char       fname[20];
  static int index     = 1;
  int        index_len = log(index) / log(10);
  snprintf(fname, 12 + index_len, "current_%i.mem", index);
  sm_mem_dump(sms_heap, fname);
  snprintf(fname, 10 + index_len, "other_%i.mem", index);
  sm_mem_dump(sms_heap, fname);
  index++;
}

// Free the heaps, preparing for closing or restarting
void sm_mem_cleanup() {
  sm_heap_set_free(sms_all_heaps);
  if (sm_global_lex_stack(NULL))
    free(sm_global_lex_stack(NULL));
}

// Print every object in current heap
void sm_sprint_dump() {
  char *scan_cursor = (char *)sms_heap->storage;
  while (scan_cursor < ((char *)sms_heap->storage) + sms_heap->used) {
    sm_object *current_obj = (sm_object *)scan_cursor;
    uint32_t   len         = sm_object_sprint(current_obj, NULL, true);
    char       buf[len];
    uint32_t   real_len = sm_object_sprint(current_obj, buf, false);
    buf[real_len]       = '\0';
    printf("%s\n", buf);
    if (current_obj->my_type <= SM_SPACE_TYPE)
      scan_cursor += sm_sizeof(current_obj);
    else {
      DEBUG("Error: Stopping on unrecognized object type.");
      return;
    }
  }
}

// Swap the ptrs to these heaps
void sm_swap_heaps(sm_heap **a, sm_heap **b) {
  sm_heap *temp = *a;
  *a            = *b;
  *b            = temp;
}
