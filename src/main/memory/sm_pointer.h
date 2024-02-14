// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

/// Copying GC's pointer, notifying where an object has been moved to
typedef struct sm_pointer {
  int        my_type;
  sm_object *address;
} sm_pointer;

/// Create a new sm_pointer object
sm_pointer *sm_new_pointer(sm_object *old_object, sm_object *address);
