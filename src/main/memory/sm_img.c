// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"

extern sm_heap     *sms_heap;
extern sm_heap     *sms_other_heap;
extern sm_symbol   *sms_true;
extern sm_symbol   *sms_false;
extern sm_heap     *sms_symbol_heap;
extern sm_heap     *sms_symbol_name_heap;
extern sm_heap_set *sms_all_heaps;


sm_object *sm_image_save(sm_string *fname_str, sm_expr *sme) {
  char *fname_cstr = &(fname_str->content);

  // Create and populate the sm_img structure with the current globals
  sm_img img;
  img.env                  = *sm_global_environment(NULL); // Copy the current environment
  img.sms_heap             = *sms_heap;                    // Copy the main heap
  img.sms_symbol_heap      = *sms_symbol_heap;             // Copy the symbol heap
  img.sms_symbol_name_heap = *sms_symbol_name_heap;        // Copy the symbol name heap

  // Perform garbage collection to compact the sms_heap
  sm_heap *compacted_heap = sm_new_heap(sms_heap->capacity, true); // Create a new heap for GC
  // TODO: sm_heap_compact_to(from,to)
  // sm_garbage_collect(sms_heap, compacted_heap); // Compact the main heap uint32_to
  // the new one

  // Open the file for writing
  FILE *file = fopen(fname_cstr, "wb");
  if (!file) {
    fprintf(stderr, "Failed to open file for writing: %s\n", fname_cstr);
    sm_symbol *title   = sm_new_symbol("imgSaveFailed", 13);
    sm_string *message = sm_new_string(36, "Failed to open for saving the image.");
    return (sm_object *)sm_new_error_from_expr(title, message, sme, NULL);
  }

  // Write the sm_img structure to the file
  if (fwrite(&img, sizeof(sm_img), 1, file) != 1) {
    fclose(file);
    fprintf(stderr, "Failed to write image to file: %s\n", fname_cstr);
    sm_symbol *title   = sm_new_symbol("imgSaveFailed", 13);
    sm_string *message = sm_new_string(26, "Failed to write the image.");
    return (sm_object *)sm_new_error_from_expr(title, message, sme, NULL);
  }

  // Write the sm_heap_set to the file
  if (fwrite(sms_all_heaps, sizeof(sm_heap_set), 1, file) != 1) {
    fclose(file);
    fprintf(stderr, "Failed to write heaps to file: %s\n", fname_cstr);
    sm_symbol *title   = sm_new_symbol("heapSaveFailed", 14);
    sm_string *message = sm_new_string(27, "Failed to write heaps data.");
    return (sm_object *)sm_new_error_from_expr(title, message, sme, NULL);
  }

  // Write the compacted sms_heap's original start point and compacted storage to the
  // file
  if (fwrite(&sms_heap->storage, sizeof(char *), 1, file) != 1 ||
      fwrite(compacted_heap->storage, compacted_heap->used, 1, file) != 1) {
    fclose(file);
    free(compacted_heap); // Free the heap using C's free function
    fprintf(stderr, "Failed to write heap data to file: %s\n", fname_cstr);
    sm_symbol *title   = sm_new_symbol("heapSaveFailed", 14);
    sm_string *message = sm_new_string(31, "Failed to write compacted heap data.");
    return (sm_object *)sm_new_error_from_expr(title, message, sme, NULL);
  }

  // Clean up
  fclose(file);
  free(compacted_heap); // Free the compacted heap after writing

  // Return sms_true to indicate success
  return (sm_object *)sms_true;
}
