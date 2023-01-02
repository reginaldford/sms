//Has (size) of sm_object* following this structure.
typedef struct sm_array{
  enum object_type my_type;
  int capacity;
  int size;
  short space; //make this struct 16 bytes
} sm_array;

sm_array  *sm_resize_array(sm_array* array,int size);
sm_array  *sm_new_array(int size, int capacity);
sm_object *sm_array_get(sm_array* array, int pos);
sm_array  *sm_array_set(sm_array* array, int pos, sm_object *value);
sm_array  *sm_new_array_with_1(sm_object *obj1);
sm_array  *sm_new_array_with_2(sm_object *obj1, sm_object *obj2);
sm_array  *sm_array_append(sm_array *array,sm_object *obj);
sm_string *sm_array_to_string(sm_array *self);

