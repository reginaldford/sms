#include "sms.h"

sm_array *sm_array_set(sm_array *array, int pos, sm_object *value) {
  sm_object **ptr_array = (sm_object **)&(array[1]);
  ptr_array[pos]        = value;
  return array;
}

sm_object *sm_array_get(sm_array *array, int pos) {
  sm_object **ptr_array = (sm_object **)&(array[1]);
  return ptr_array[pos];
}

sm_array *sm_new_array(int size, int capacity) {
  sm_array *new_array = (sm_array *)sm_malloc(sizeof(sm_array) + (sizeof(void *)) * (capacity));
  new_array->my_type  = sm_array_type;
  new_array->size     = size;
  new_array->capacity = capacity;
  return new_array;
}

sm_array *sm_new_array_with_1(sm_object *obj1) {
  sm_array *new_array = sm_new_array(1, 1);
  sm_array_set(new_array, 0, obj1);
  return new_array;
}

sm_array *sm_new_array_with_2(sm_object *obj1, sm_object *obj2) {
  sm_array *new_array = sm_new_array(2, 2);
  sm_array_set(new_array, 0, obj1);
  sm_array_set(new_array, 1, obj2);
  return new_array;
}

sm_array *sm_resize_array(sm_array *array, int new_capacity) {
  sm_array *new_array = (sm_array *)sm_realloc(
      (sm_object *)array, sizeof(sm_array) + (sizeof(void *)) * (new_capacity));
  new_array->capacity = new_capacity;
  return new_array;
}

sm_array *sm_array_append(sm_array *array, sm_object *obj) {
  if (array->size == array->capacity) {
    int cap             = array->capacity;
    int new_capacity    = (int)(sm_collection_growth_factor(0) * cap) + 1;
    sm_array *new_array = sm_resize_array(array, new_capacity);
    new_array->size += 1;
    return sm_array_set(new_array, new_array->size - 1, obj);
  } else {
    array->size += 1;
    return sm_array_set(array, array->size - 1, obj);
  }
}

sm_string *sm_array_to_string(sm_array *self) {
  if (self->size == 0) {
    sm_string *answer = sm_new_string(3, "[ ]");
    return answer;
  }
  int object_length_sum = 0;
  sm_string *object_strings[self->size];
  for (int object_index = 0; object_index < self->size; object_index++) {
    sm_object *current_object    = sm_array_get(self, object_index);
    object_strings[object_index] = sm_object_to_string(current_object);
    object_length_sum += object_strings[object_index]->size;
  }
  int answer_length = 2 + object_length_sum + self->size * 2 + 1;
  char *buf         = malloc(sizeof(char) * answer_length);
  sprintf(buf, "[ ");
  int buffer_pos = 2;
  for (int object_index = 0; object_index < self->size - 1; object_index++) {
    sprintf(buf + buffer_pos, "%s, ", &(object_strings[object_index]->content));
    //+2 for space on the left and comma on the right
    buffer_pos += object_strings[object_index]->size + 2;
  }
  sprintf(buf + buffer_pos, "%s ]", &(object_strings[self->size - 1]->content));
  buffer_pos += object_strings[self->size - 1]->size + 2;
  sm_string *answer = sm_new_string(buffer_pos, buf);
  free(buf);
  return answer;
}
