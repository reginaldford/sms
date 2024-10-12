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
  sm_heap *new_heap = (sm_heap *)malloc(sizeof(sm_heap) + capacity);
  if (new_heap == NULL) {
    fprintf(stderr, "Cannot allocate memory for heap. %s:%i", __FILE__, __LINE__);
    exit(1);
  }
  new_heap->capacity = capacity;
  new_heap->used     = 0;
  new_heap->storage  = (char *)(new_heap + 1);
  sm_heap_set_add(sms_all_heaps, new_heap);
  if (map) {
    uint64_t size = (capacity + 63) / 64;
    new_heap->map = malloc(size);
    if (new_heap->map == NULL) {
      fprintf(stderr, "Cannot allocate memory for heap map. %s:%i", __FILE__, __LINE__);
      exit(1);
    }
    memset(new_heap->map, 0, size);
  }
  return new_heap;
}

void *sm_malloc_at(sm_heap *h, uint32_t size) {
  uint32_t bytes_used      = h->used;
  uint32_t next_bytes_used = h->used + size;
  if (next_bytes_used > h->capacity) {
    fprintf(stderr, "Cannot allocate space for an object in this heap. %s:%i", __FILE__, __LINE__);
  }
  // Mark the map if necessary
  if (h->map) {
    uint32_t map_pos  = h->used / 8;       // Each bit represents 8 bytes
    uint32_t byte_pos = map_pos / 8;       // Find the byte in the bitmap
    uint32_t bit_pos  = 7 - (map_pos % 8); // Find the specific bit in the byte
    h->map[byte_pos] |= (1 << bit_pos);    // Set the appropriate bit
  }
  // Final essentials
  h->used = next_bytes_used;
  return (void *)(((char *)h->storage) + bytes_used);
}


// Internal 'malloc'
void *sm_malloc(uint32_t size) {
  uint32_t bytes_used      = sms_heap->used;
  uint32_t next_bytes_used = sms_heap->used + size;
  if (next_bytes_used > sms_heap->capacity) {
    // Cleanup
    if (!sms_other_heap)
      sms_other_heap = sm_new_heap(sms_heap->capacity, true);
    memory_marker2 = __builtin_frame_address(1);
    sm_garbage_collect(sms_heap, sms_other_heap);
    // Swap heaps
    sm_swap_heaps(&sms_heap, &sms_other_heap);
    sms_other_heap->used = 0;
    next_bytes_used      = sms_heap->used + size;
    if (next_bytes_used > sms_heap->capacity) {
      fprintf(stderr, "Exhausted heap memory.\n");
      exit(1);
    }
  }
  // For heaps that aren't the main 2
  if (next_bytes_used > sms_heap->capacity) {
    fprintf(stderr, "Cannot allocate space for an object in this heap. %s:%i", __FILE__, __LINE__);
  }
  // Mark the map if necessary
  if (sms_heap->map) {
    uint32_t map_pos  = sms_heap->used / 8;    // Each bit represents 8 bytes
    uint32_t byte_pos = map_pos / 8;           // Find the byte in the bitmap
    uint32_t bit_pos  = 7 - (map_pos % 8);     // Find the specific bit in the byte
    sms_heap->map[byte_pos] |= (1 << bit_pos); // Set the appropriate bit
  }
  // Final essentials
  sms_heap->used = next_bytes_used;
  return (void *)(((char *)sms_heap->storage) + bytes_used);
}

void printBinary(unsigned char byte) {
  for (int i = 7; i >= 0; i--) {
    printf("%u", (byte >> i) & 1);
  }
}

// Return whether this pointer aims at the beginning of a registered heap object
bool sm_heap_has_object(sm_heap *heap, void *guess) {
  // First check if it's in bounds
  if (!sm_is_within_heap(guess, heap))
    return false;

  // Calculate the position in the bitmap
  uint32_t map_pos =
    ((intptr_t *)guess - (intptr_t *)heap->storage) / 8; // Offset in the heap divided by 8 bytes
  uint32_t byte_pos = map_pos / 8;                       // Find the byte in the bitmap
  uint32_t bit_pos  = 7 - (map_pos % 8);                 // Find the specific bit in the byte
  // Check if the bit is set in the bitmap
  // printf("\nmap pos: %u\n", map_pos);
  // for (int i = 0; i < heap->used / 64; i++)
  //  printBinary(heap->map[i]);
  // Print a newline for the next line
  // Print dots and caret to indicate the bit being checked
  // printf("\n");
  // for (int i = 0; i < byte_pos * 8 + bit_pos; i++)
  //  printf(".");
  // printf("^\n"); // Point to the bit
  // fflush(stdout);
  return heap->map[byte_pos] & (1 << bit_pos);
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
  if (!h)
    return;                           // Safety check for NULL
  memset(h->storage, 0, h->capacity); // Clear the heap storage
  h->used = 0;
  if (h->map) {
    memset(h->map, 0,
           (h->capacity + 63) / 64); // Clear the map (1 bit per 8 bytes, i.e., 1/64 of capacity)
  }
}


// Is the ptr within this heap?
bool sm_is_within_heap(void *ptr, sm_heap *heap) {
  return (ptr >= (void *)heap->storage) && (ptr < (void *)((char *)heap->storage) + heap->used);
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
