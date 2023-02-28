// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "sms.h"

// Return true if the object is a literal
// This means the object evaluates to itself
bool sm_object_is_literal(unsigned short int t) {
  switch (t) {
  case sm_double_type:
  case sm_meta_expr:
  case sm_string_expr:
    return true;
  default:
    return false;
  }
}

// Return the object type as a string.
// Keep syncronized with sms.h sm_object_expr list
sm_string *sm_object_expr_str(unsigned short int t) {
  static char *response_string[]     = {"double",  "expression", "primitive", "string", "symbol",
                                        "context", "pointer",    "key_value", "meta",   "space",
                                        "fun",     "fun_param",  "local",     "?"};
  static int   response_string_len[] = {6, 10, 9, 6, 6, 7, 7, 9, 4, 5, 3, 5, 1};
  if (t < ARRAY_SIZE(response_string_len))
    return sm_new_string(response_string_len[t], response_string[t]);
  else
    return sm_new_string(10, "NoSuchType");
}

// Return a new sm_string describing the object
sm_string *sm_object_to_string(sm_object *obj1) {
  sm_string   *new_str     = sm_new_string_manual(sm_object_sprint(obj1, NULL, true));
  unsigned int len         = sm_object_sprint(obj1, &(new_str->content), false);
  (&new_str->content)[len] = '\0';
  return new_str;
}

// Print to a string buffer the object description
// Return the length of the string
unsigned int sm_object_sprint(sm_object *obj1, char *buffer, bool fake) {
  switch (obj1->my_type) {
  case sm_double_type:
    return sm_double_sprint((sm_double *)obj1, buffer, fake);
  case sm_string_expr:
    return sm_string_sprint((sm_string *)obj1, buffer, fake);
  case sm_expr_type:
    return sm_expr_sprint((sm_expr *)obj1, buffer, fake);
  case sm_meta_expr:
    return sm_meta_sprint((sm_meta *)obj1, buffer, fake);
  case sm_context_type:
    return sm_context_sprint((sm_context *)obj1, buffer, fake);
  case sm_symbol_type:
    return sm_symbol_sprint((sm_symbol *)obj1, buffer, fake);
  case sm_fun_expr:
    return sm_fun_sprint((sm_fun *)obj1, buffer, fake);
  case sm_local_type:
    return sm_local_sprint((sm_local *)obj1, buffer, fake);
  case sm_error_expr:
    return sm_error_sprint((sm_error *)obj1, buffer, fake);
  default: {
    if (!fake)
      sprintf(buffer, "?(%i)", obj1->my_type);
    return 3 + sm_double_len((double)obj1->my_type);
  }
  }
}

// Return the size of the object in bytes
int sm_sizeof(sm_object *obj1) {
  switch (obj1->my_type) {
  case sm_double_type:
    return sizeof(sm_double);
  case sm_expr_type:
    return sizeof(sm_expr) + sizeof(sm_object *) * ((sm_expr *)obj1)->capacity;
  case sm_primitive_type:
    return sizeof(sm_expr);
  case sm_string_expr:
    return sm_round_size(sizeof(sm_string) + ((sm_string *)obj1)->size + 1);
  case sm_symbol_type:
    return sizeof(sm_symbol);
  case sm_context_type:
    return sizeof(sm_context) + sizeof(sm_context_entry) * ((sm_context *)obj1)->capacity;
  case sm_pointer_expr:
    return sizeof(sm_pointer);
  case sm_meta_expr:
    return sizeof(sm_meta);
  case sm_fun_expr:
    return sizeof(sm_fun) + sizeof(sm_fun_param) * ((sm_fun *)obj1)->num_params;
  case sm_fun_param_expr:
    return sizeof(sm_fun_param_obj);
  case sm_local_type:
    return sizeof(sm_local);
  case sm_space_expr:
    return ((sm_space *)obj1)->size;
  case sm_error_expr:
    return sizeof(sm_error);
  default:
    printf("Cannot determine size of object of type %d\n", obj1->my_type);
    exit(0);
  }
}
