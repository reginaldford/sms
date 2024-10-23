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
sm_heap *sm_new_heap(uint32_t capacity, bool map) {
  sm_heap *new_heap = (sm_heap *)malloc(sizeof(sm_heap) + sm_round_size64(capacity));
  if (new_heap == NULL) {
    fprintf(stderr, "Cannot allocate memory for heap. %s:%i", __FILE__, __LINE__);
    exit(1);
  }
  new_heap->capacity = sm_round_size64(capacity);
  new_heap->storage  = (char *)(new_heap + 1);
  sm_heap_set_add(sms_all_heaps, new_heap);
  if (map) {
    uint64_t map_size = (capacity + 63) / 64;
    new_heap->map     = malloc(map_size);
    if (new_heap->map == NULL) {
      fprintf(stderr, "Cannot allocate memory for heap map. %s:%i", __FILE__, __LINE__);
      exit(1);
    }
  }
  sm_heap_clear(new_heap);
  return new_heap;
}

void *sm_malloc_at(sm_heap *h, uint32_t size) {
  uint32_t bytes_used      = h->used;
  uint32_t next_bytes_used = h->used + size;
  // Check for sufficient capacity
  if (next_bytes_used >= h->capacity) {
    // Only GC sms_heap
    if (h == sms_heap) {
      // Try gc
      sm_garbage_collect();
      // Refresh values
      h               = sms_heap;
      bytes_used      = h->used;
      next_bytes_used = bytes_used + size;
      if (next_bytes_used >= h->capacity) {
        fprintf(stderr, "Ran out of heap memory. Try with more memory (sms -h for help)");
        exit(1);
      }
    } else {
      fprintf(stderr, "Ran out of heap memory. Try with more memory (sms -h for help)");
      exit(1);
    }
  }
  // Update the used bytes
  h->used = next_bytes_used;
  // Get the pointer to the newly allocated space
  void *allocated_space = (void *)(((char *)h->storage) + bytes_used);
  // Give this chunk a valid sm_space header so that it survives gc
  memset(allocated_space, 0, size);
  return allocated_space; // Return the pointer to the allocated space
}

// Internal 'malloc'
void *sm_malloc(uint32_t size) { return sm_malloc_at(sms_heap, size); }

// Print this heap's map in binary form
void sm_heap_print_map(sm_heap *h) {
  uint64_t map_size = (h->capacity + 63) / 64;
  for (uint32_t i = 0; i < map_size; i++)
    for (int j = 7; i >= 0; i--)
      printf("%u", (h->map[i] >> j) & 1);
}

// Return whether this pointer aims at the beginning of a registered heap object
bool sm_heap_has_object(sm_heap *heap, void *guess) {
  // First check if it's aligned and in bounds
  if (((intptr_t)guess & 7) || !sm_is_within_heap(guess, heap))
    return false;
  // Calculate the position in the bitmap
  uint32_t map_pos =
    ((intptr_t *)guess - (intptr_t *)heap->storage) >> 3; // Offset in the heap divided by 8 bytes
  uint32_t byte_pos = map_pos >> 3;                       // Find the byte in the bitmap
  uint8_t  bit_pos  = map_pos & 7;                        // Find the specific bit in the byte
  return heap->map[byte_pos] & (1 << bit_pos);
}

// Designed to be fast
void sm_heap_register_object(sm_heap *heap, void *obj) {
  if (((intptr_t)obj) & 7) {
    fprintf(stderr, "Misaligned object registration. File: %s Line: %u\n", __FILE__, __LINE__);
    exit(1);
  }
  // Calculate the position in the bitmap
  uint32_t map_pos =
    ((intptr_t *)obj - (intptr_t *)heap->storage) >> 3; // Offset in the heap divided by 8 bytes
  uint32_t byte_pos = map_pos >> 3;                     // Find the byte in the bitmap
  uint32_t bit_pos  = map_pos & 7;                      // Find the specific bit in the byte
  heap->map[byte_pos] |= (1 << bit_pos);
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

// Clear the heap and set used to 0
void sm_heap_clear(struct sm_heap *h) {
  h->used = 0;
  if (h->map) {
    memset(h->map, 0,
           h->capacity / 64); // Clear the map (1 bit per 8 bytes, i.e., 1/64 of capacity)
  }
}

// Is the ptr within this heap?
bool sm_is_within_heap(void *ptr, sm_heap *heap) {
  return (ptr >= (void *)heap->storage) && (ptr < (void *)(heap->storage) + heap->used);
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
  sm_mem_dump(sms_other_heap, fname);
  index++;
}

// Free the heaps, preparing for closing or restarting
void sm_mem_cleanup() { sm_heap_set_free(sms_all_heaps); }

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

// Scan the heap and generate the bitmap. Return true on success.
bool sm_heap_scan(sm_heap *h) {
  sm_object *obj      = (sm_object *)((intptr_t)h->storage);
  sm_object *prev_obj = NULL; // Initialize prev_obj to track the previous object
  while ((char *)obj < h->storage + h->used) {
    // Register in heap map if it has valid object size
    uint32_t sizeOfObj = sm_sizeof(obj);
    if (!sizeOfObj || sizeOfObj & 7 || obj->my_type == SM_POINTER_TYPE) {
      fprintf(stderr, "! Corrupt object. File: %s , line: %u\n", __FILE__, __LINE__);
      return false;
    }
    sm_heap_register_object(sms_heap, obj);
    // Move to the next object
    prev_obj = obj; // Update prev_obj to the current object
    obj      = (sm_object *)((char *)obj + (sizeOfObj));
  }
  return true;
}
