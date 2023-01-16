// The following file is provided under the BSD 2-clause license. For more info, read LICENSE.txt.

// all garbage collected objects start with this
typedef struct sm_object {
  short int my_type;
} sm_object;

sm_string   *sm_object_type_str(unsigned short int t);
sm_string   *sm_object_to_string(sm_object *obj1);
unsigned int sm_object_sprint(sm_object *obj1, char *buffer);
int          sm_sizeof(sm_object *sm_obj);
unsigned int sm_object_to_string_len(sm_object *obj1);
