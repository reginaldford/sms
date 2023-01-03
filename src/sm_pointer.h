typedef struct sm_pointer {
  int        my_type;
  sm_object *address;
} sm_pointer;

sm_pointer *sm_new_pointer(sm_object *old_object, sm_object *address);
