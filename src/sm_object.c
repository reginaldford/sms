// This project is licensed under the BSD 2 clause license. See LICENSE.txt for more information.

#include "sms.h"

sm_string *sm_object_type_str(enum sm_object_type t) {
  char *response_string[]     = {"double",  "expression", "primitive", "string", "symbol",
                                 "context", "pointer",    "key_value", "meta",   "space"};
  int   response_string_len[] = {7, 10, 9, 6, 6, 7, 3, 9, 4, 6};
  if (t >= 0 && t < sizeof(response_string) / sizeof(void *))
    return sm_new_string(response_string_len[t], response_string[t]);
  else
    return sm_new_string(10, "NoSuchType");
}

sm_string *sm_object_to_string(sm_object *obj1) {
  enum sm_object_type t = obj1->my_type;

  if (t == sm_double_type) {
    return sm_double_to_string((sm_double *)obj1);
  } else if (t == sm_string_type) {
    return sm_string_to_string((sm_string *)obj1);
  } else if (t == sm_expression_type) {
    return sm_expression_to_string((sm_expression *)obj1);
  } else if (t == sm_symbol_type) {
    return sm_symbol_to_string((sm_symbol *)obj1);
  } else if (t == sm_context_type) {
    return sm_context_to_string((sm_context *)obj1);
  } else if (t == sm_meta_type) {
    return sm_meta_to_string((sm_meta *)obj1);
  } else if (t == sm_space_type) {
    return sm_space_to_string((sm_space *)obj1);
  } else {
    return sm_new_string(5, "other");
  }
}

int sm_sizeof(sm_object *obj1) {
  enum sm_object_type obj_type = obj1->my_type;
  switch (obj_type) {
  case sm_double_type:
    return sizeof(sm_double);
    break;
  case sm_expression_type:
    return sizeof(sm_expression) + sizeof(sm_object *) * ((sm_expression *)obj1)->size;
    break;
  case sm_primitive_type:
    return sizeof(sm_expression);
    break;
  case sm_string_type:
    return sm_round_size(sizeof(sm_string) + ((sm_string *)obj1)->size + 1);
    break;
  case sm_symbol_type:
    return sizeof(sm_symbol);
    break;
  case sm_context_type:
    return sizeof(sm_context) + sizeof(sm_context_entry) * ((sm_context *)obj1)->capacity;
    break;
  case sm_pointer_type:
    return sizeof(sm_pointer);
    break;
  case sm_meta_type:
    return sizeof(sm_meta);
    break;
  case sm_space_type:
    return ((sm_space *)obj1)->size;
    break;

  default:
    printf("Cannot determine size of object of type %d\n", obj_type);
    exit(0);
  }
}
