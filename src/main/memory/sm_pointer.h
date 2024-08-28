// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

/// Copying GC's pointer, notifying where an object has been moved to
/// The location is relative to the heap, with 32bits meaning the max heap size is 4.2 billion bytes
typedef struct sm_pointer {
  uint32_t my_type;
  uint32_t address;
} sm_pointer;

/// Create a new sm_pointer object
sm_pointer *sm_new_pointer(sm_heap *origin, sm_object *old_object, sm_object *address);
