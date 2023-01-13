// The following file is provided under the BSD 2-clause license. For more info, read LICENSE.txt.

// all garbage collected objects start with this
typedef struct sm_object {
  enum sm_object_type my_type;
} sm_object;

sm_string *sm_object_type_str(enum sm_object_type t);
sm_string *sm_object_to_string(sm_object *obj1);
int        sm_sizeof(sm_object *sm_obj);
