// This project is licensed under the BSD 2 clause license. See LICENSE.txt for more information.

#include "sms.h"

int sm_round_size(int size) { return ((size) + 3) & ~3; }

sm_memory_heap *sm_new_memory_heap(unsigned int capacity) {
  sm_memory_heap *new_heap = (sm_memory_heap *)malloc(sizeof(sm_memory_heap));
  new_heap->capacity       = capacity;
  new_heap->used           = 0;
  new_heap->storage        = malloc(capacity);
  return new_heap;
}

void *sm_malloc(unsigned int size) {
  if (sm_global_current_heap(NULL)->used + size > sm_global_current_heap(NULL)->capacity) {
    printf("We ran out of memory. gc time.\n");
    fflush(stdout);
    sm_garbage_collect();
    // TODO: check if space is good, if not, implement heap compaction
    return sm_malloc(size); // This can turn into an infinite loop!
  } else {
    unsigned int bytes_used = sm_global_current_heap(NULL)->used;
    sm_global_current_heap(NULL)->used += size;
    return (void *)(((char *)sm_global_current_heap(NULL)->storage) + bytes_used);
  }
}

sm_object *sm_realloc(sm_object *obj, unsigned int size) {
  sm_object *new_space = sm_malloc(size);
  return memcpy(new_space, obj, size);
}

bool sm_is_within_heap(sm_object *obj, sm_memory_heap *heap) {
  return ((void *)obj >= (void *)heap) && ((void *)obj < (void *)((char *)heap) + heap->used);
}

int sm_mem_dump(sm_memory_heap *heap, char *name) {
  void *buffer        = heap->storage;
  int   buffer_length = heap->used;

  // Open a file for writing
  FILE *fp = fopen(name, "wb");
  // Write the data to the file
  fwrite(buffer, 1, buffer_length, fp);
  // Close the file
  // returns 0 if there are no problems.
  return fclose(fp);
}

void sm_dump_and_count() {
  char       fname[20];
  static int index = 0;
  sprintf(fname, "current_%i.mem", index);
  sm_mem_dump(sm_global_current_heap(NULL), fname);
  sprintf(fname, "other_%i.mem", index);
  sm_mem_dump(sm_global_current_heap(NULL), fname);
}

void sm_mem_cleanup() {
  if (sm_global_current_heap(NULL) != NULL)
    free(sm_global_current_heap(NULL));
  if (sm_global_other_heap(NULL) != NULL)
    free(sm_global_other_heap(NULL));
}
