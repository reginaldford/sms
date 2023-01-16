// The following file is provided under the BSD 2-clause license. For more info, read LICENSE.txt.

#include "sms.h"

sm_string *sm_object_type_str(unsigned short int t) {
  static char *response_string[]     = {"double",  "expression", "primitive", "string", "symbol",
                                        "context", "pointer",    "key_value", "meta",   "space"};
  static int   response_string_len[] = {6, 10, 9, 6, 6, 7, 7, 9, 4, 5};
  if (t < ARRAY_SIZE(response_string_len))
    return sm_new_string(response_string_len[t], response_string[t]);
  else
    return sm_new_string(10, "NoSuchType");
}

// Return a new sm_string describing the object
sm_string *sm_object_to_string(sm_object *obj1) {
  sm_string *new_str = sm_new_string(sm_object_to_string_len(obj1), "");
  sm_object_sprint(obj1, &(new_str->content));
  (&new_str->content)[new_str->size] = '\0';
  return new_str;
}

// Return the exact length of string that sm_context_entry would produce
unsigned int sm_object_to_string_len(sm_object *obj1) {
  switch (obj1->my_type) {
  case sm_double_type:
    return sm_double_to_string_len((sm_double *)obj1);
  case sm_string_type:
    return ((sm_string *)obj1)->size + 2;
  case sm_expr_type:
    return sm_expr_to_string_len((sm_expr *)obj1);
  case sm_context_type:
    return sm_context_to_string_len((sm_context *)obj1);
  case sm_meta_type:
    return sm_meta_to_string_len((sm_meta *)obj1);
  case sm_symbol_type:
    return sm_symbol_to_string_len((sm_symbol *)obj1);
  default:
    return 5; //'other'
  }
}

// Return a new sm_cstring describing the object
unsigned int sm_object_sprint(sm_object *obj1, char *buffer) {
  switch (obj1->my_type) {
  case sm_double_type:
    return sm_double_sprint((sm_double *)obj1, buffer);
  case sm_string_type:
    return sm_string_sprint((sm_string *)obj1, buffer);
  case sm_expr_type:
    return sm_expr_sprint((sm_expr *)obj1, buffer);
  case sm_meta_type:
    return sm_meta_sprint((sm_meta *)obj1, buffer);
  case sm_context_type:
    return sm_context_sprint((sm_context *)obj1, buffer);
  case sm_symbol_type:
    return sm_symbol_sprint((sm_symbol *)obj1, buffer);
    /*
  case sm_space_type:
    return sm_space_to_cstring((sm_space *)obj1);
    */
  default:
    sm_strncpy(buffer, "other", 5);
    return 5;
  }
}

// Return the size of the object in bytes
int sm_sizeof(sm_object *obj1) {
  short int obj_type = obj1->my_type;
  if (obj_type == sm_double_type)
    return sizeof(sm_double);
  if (obj_type == sm_expr_type)
    return sizeof(sm_expr) + sizeof(sm_object *) * ((sm_expr *)obj1)->size;
  if (obj_type == sm_primitive_type)
    return sizeof(sm_expr);
  if (obj_type == sm_string_type)
    return sm_round_size(sizeof(sm_string) + ((sm_string *)obj1)->size + 1);
  if (obj_type == sm_symbol_type)
    return sizeof(sm_symbol);
  if (obj_type == sm_context_type)
    return sizeof(sm_context) + sizeof(sm_context_entry) * ((sm_context *)obj1)->capacity;
  if (obj_type == sm_pointer_type)
    return sizeof(sm_pointer);
  if (obj_type == sm_meta_type)
    return sizeof(sm_meta);
  if (obj_type >= sm_space_type)
    return ((sm_space *)obj1)->my_type - sm_space_type;

  printf("Cannot determine size of object of type %d\n", obj_type);
  sm_mem_dump(sm_global_current_heap(NULL), "memory.dat");
  exit(0);
}
