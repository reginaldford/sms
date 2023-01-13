// The following file is provided under the BSD 2-clause license. For more info, read LICENSE.txt.

#include "sms.h"

// For rounding up object size to the next multiple of 4 bytes.
int sm_round_size(int size) { return ((size) + 3) & ~3; }

// Create a new heap of some capacity
sm_heap *sm_new_heap(unsigned int capacity) {
  sm_heap *new_heap  = (sm_heap *)malloc(sizeof(sm_heap));
  new_heap->capacity = capacity;
  new_heap->used     = 0;
  new_heap->storage  = malloc(capacity);
  return new_heap;
}

// Check for adequate space size
// If it is acceptable,  delete it from the space array and return the space, else return NULL
sm_space *check_space(unsigned int size, unsigned int index) {
  if (index + 1 <= sm_global_space_array(NULL)->size) {
    unsigned int space_size = sm_get_space_array(sm_global_space_array(NULL))[index]->size;
    if (space_size >= size) {
      sm_space *good_space = sm_get_space_array(sm_global_space_array(NULL))[index];
      return good_space;
    }
    return NULL;
  }
  return NULL;
}

// Binary search for space in a size ordered space array
search_result find_space_within_bounds(sm_space_array *ssa, unsigned int size,
                                       unsigned int lower_limit, unsigned int upper_limit) {
  sm_space   **space_array = sm_get_space_array(ssa);
  int          comparison  = 1;
  unsigned int guess_point = (upper_limit + lower_limit) / 2.0;
  while (lower_limit < upper_limit && comparison != 0) {
    comparison = space_array[guess_point]->size - size;
    if (comparison == 0)
      return (search_result){.found = true, .index = guess_point};
    else if (comparison > 0)
      upper_limit = guess_point == 0 ? 0 : guess_point - 1;
    else
      lower_limit = guess_point + 1;
    guess_point = (upper_limit + lower_limit) / 2.0;
  }
  comparison = space_array[guess_point]->size - size;
  if (comparison == 0)
    return (search_result){.found = true, .index = guess_point};
  if (comparison < 0) {
    return (search_result){.found = false, .index = guess_point + 1};
  } else { // comparison > 0
    return (search_result){.found = false, .index = guess_point};
  }
}

// Sort the space array that is unsorted by the 1 space at off_index
void sort_1_off(sm_space_array *ssa, unsigned int off_index) {
  unsigned int space_size = sm_get_space_array(ssa)[off_index]->size;
  // Now, to binary search the remaining portion of the array
  search_result sr = find_space_within_bounds(ssa, space_size, 0, off_index - 1);
  // Use search result to sort this 1-off array
  sm_space   **space_array = sm_get_space_array(ssa);
  sm_space    *off_space   = space_array[off_index];
  unsigned int upper_index = off_index;
  unsigned int lower_index = sr.index;
  for (unsigned int i = upper_index; i > lower_index; i--) {
    space_array[i] = space_array[i - 1];
  }
  space_array[lower_index] = off_space;
}

// Internal 'malloc'. checks space array first, else move the 'used' integer up
void *sm_malloc(unsigned int size) {
  if (sm_global_space_array(NULL)->size > 0) {
    search_result sr = sm_space_array_find(sm_global_space_array(NULL), size);
    // sr.found is true for exact matches
    if (sr.found == true) {
      sm_space *good_space = sm_get_space_array(sm_global_space_array(NULL))[sr.index];
      // Deleting space by its index
      sm_delete_space_by_index(sm_global_space_array(NULL), sr.index);
      return good_space;
    }
    sm_space *result_space = check_space(size, sr.index);
    if (result_space != NULL) {
      unsigned int remaining_size = result_space->size - size;
      if (remaining_size >= sizeof(sm_space)) {
        sm_space *new_space = (sm_space *)((char *)result_space) + size;
        new_space->my_type  = sm_space_type;
        new_space->size     = remaining_size;
        sm_get_space_array(sm_global_space_array(NULL))[sr.index] = new_space;
        sort_1_off(sm_global_space_array(NULL), sr.index);
      } else {
        sm_delete_space_by_index(sm_global_space_array(NULL), sr.index);
      }
      return result_space;
    }
  }
  // If no spaces were found, default to classic copy gc allocation
  unsigned int bytes_used = sm_global_current_heap(NULL)->used;
  sm_global_current_heap(NULL)->used += size;
  return (void *)(((char *)sm_global_current_heap(NULL)->storage) + bytes_used);
}

// Reallocate memory space for resizing or recreating objects
sm_object *sm_realloc(sm_object *obj, unsigned int size) {
  sm_object *new_space = sm_malloc(size);
  return memcpy(new_space, obj, size);
}

// Is the object within this heap?
bool sm_is_within_heap(sm_object *obj, sm_heap *heap) {
  return ((void *)obj >= (void *)heap) && ((void *)obj < (void *)((char *)heap) + heap->used);
}

// For advanced debugging, run this at any point and examine the heap snapshot as a file
int sm_mem_dump(sm_heap *heap, char *name) {
  void *buffer        = heap->storage;
  int   buffer_length = heap->used;

  // Open a file for writing
  FILE *fp = fopen(name, "wb");
  // Write the data to the file
  fwrite(buffer, 1, buffer_length, fp);
  // Close the file
  // Returns 0 if there are no problems
  return fclose(fp);
}

// Take multiple snapshots of both heaps for inflation debugging
void sm_dump_and_count() {
  char       fname[20];
  static int index = 0;
  sprintf(fname, "current_%i.mem", index);
  sm_mem_dump(sm_global_current_heap(NULL), fname);
  sprintf(fname, "other_%i.mem", index);
  sm_mem_dump(sm_global_current_heap(NULL), fname);
}

// Free the heaps, preparing for closing or restarting
void sm_mem_cleanup() {
  if (sm_global_current_heap(NULL) != NULL)
    free(sm_global_current_heap(NULL));
  if (sm_global_other_heap(NULL) != NULL)
    free(sm_global_other_heap(NULL));
}
