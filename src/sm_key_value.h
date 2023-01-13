// The following file is provided under the BSD 2-clause license. For more info, read LICENSE.txt.

typedef struct sm_key_value {
  enum sm_object_type my_type;
  sm_string          *name;
  sm_object          *value;
} sm_key_value;

sm_key_value *sm_new_key_value(sm_string *key, sm_object *value);
