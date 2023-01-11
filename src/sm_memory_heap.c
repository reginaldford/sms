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

sm_space *check_space(unsigned int size, unsigned int index) {
  if (index + 1 <= sm_global_space_array(NULL)->size) {
    unsigned int space_size = sm_get_space_array(sm_global_space_array(NULL))[index]->size;
    if (space_size >= size) {
      sm_space *good_space = sm_get_space_array(sm_global_space_array(NULL))[index];
      sm_delete_space_by_index(sm_global_space_array(NULL), index);
      // making the new space for the remaining space
      unsigned int remaining_size = space_size > size ? space_size - size : 0;
      if (remaining_size > 0)
        sm_new_space((((char *)good_space) + size), remaining_size);
      return good_space;
    }
    return NULL;
  }
  return NULL;
}

void *sm_malloc(unsigned int size) {
  if (sm_global_space_array(NULL)->size > 0) {
    search_result sr = sm_space_array_find(sm_global_space_array(NULL), size);
    if (sr.found == true) {
      sm_space *good_space = sm_get_space_array(sm_global_space_array(NULL))[sr.index];
      // deleting space by its index
      sm_delete_space_by_index(sm_global_space_array(NULL), sr.index);
      return good_space;
    }
    sm_space *result_space = check_space(size, sr.index);
    if (result_space != NULL) {
      return result_space;
    } else {
      // try higher
      if (sr.index + 2 <= sm_global_space_array(NULL)->size) {
        sm_space *result_space = check_space(size, sr.index + 1);
        if (result_space != NULL) {
          return result_space;
        }
      }
      // try lower
      if (sr.index > 0) {
        sm_space *result_space = check_space(size, sr.index - 1);
        if (result_space != NULL) {
          return result_space;
        }
      }
    }
  }
  // If no spaces were found, fall back to classic copy gc allocation
  unsigned int bytes_used = sm_global_current_heap(NULL)->used;
  sm_global_current_heap(NULL)->used += size;
  return (void *)(((char *)sm_global_current_heap(NULL)->storage) + bytes_used);
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
