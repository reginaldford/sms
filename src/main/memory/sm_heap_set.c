// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information
#include "../sms.h"

// Set of heaps
sm_heap_set *sm_new_heap_set(uint64_t capacity, uint64_t size, sm_heap **heaps) {
  sm_heap_set *new_set = malloc(sizeof(sm_heap_set) + sizeof(sm_heap *) * sm_round_size(capacity));
  new_set->capacity    = capacity;
  new_set->size        = size;
  new_set->heaps       = (sm_heap **)(&(new_set[1]));
  for (uint64_t i = 0; i < size; i++)
    new_set->heaps[i] = heaps[i];
  return new_set;
}

// Add a heap to the set , which might result in reallocation
sm_heap_set *sm_heap_set_add(sm_heap_set *hs, sm_heap *newbie) {
  if (hs->size == hs->capacity)
    hs = sm_realloc(hs, 1 + hs->capacity * sm_global_growth_factor(0));
  hs->heaps[hs->size] = newbie;
  hs->size++;
  return hs;
}

// Frees the memory associated with a heap set and its heaps
bool sm_heap_set_free(sm_heap_set *hs) {
  if (hs == NULL)
    return false; // Return false if the heap set is NULL
  // Free each heap in the set
  // The heap ptr is 1 sizeof(heap) above the malloc/free ptr, so we subtract 1 back off
  for (uint64_t i = 0; i < hs->size; i++)
    if (hs->heaps[i] != NULL)
      free(hs->heaps[i] - 1); // Assuming sm_heap_free is defined to free individual heaps
  // Free the heap set itself
  free(hs);
  return true; // Return true indicating successful deallocation
}

// Removes a heap from the heap set
bool sm_heap_set_rm(sm_heap *goner, sm_heap_set *hs) {
  if (hs == NULL || goner == NULL)
    return false; // Return false if the heap set or the heap to remove is NULL
  bool found = false;
  // Find the heap to remove in the set
  for (uint64_t i = 0; i < hs->size; i++) {
    if (hs->heaps[i] == goner) {
      found = true;
      free(goner); // Free the heap to be removed
      // Shift remaining heaps to fill the gap
      for (uint64_t j = i; j < hs->size - 1; j++)
        hs->heaps[j] = hs->heaps[j + 1];
      hs->size--; // Decrement the size of the heap set
      break;      // Exit the loop since we found and removed the heap
    }
  }
  return found; // Return true if the heap was found and removed, false otherwise
}
