// The following file is provided under the BSD 2-clause license. For more info, read LICENSE.txt.
typedef struct sm_pointer {
  int        my_type;
  sm_object *address;
} sm_pointer;

sm_pointer *sm_new_pointer(sm_object *old_object, sm_object *address);
