// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "sms.h"

// Return true if the object is a literal
// This means the object evaluates to itself
bool sm_object_is_literal(unsigned short int t) {
  switch (t) {
  case sm_double_type:
  case sm_meta_type:
  case sm_string_type:
    return true;
  default:
    return false;
  }
}

// Return the object type as a string.
// Keep syncronized with sms.h sm_object_type list
sm_string *sm_object_type_str(unsigned short int t) {
  static char *response_string[]     = {"double",  "expression", "primitive", "string", "symbol",
                                        "context", "pointer",    "key_value", "meta",   "space",
                                        "fun",     "fun_param",  "local",     "?"};
  static int   response_string_len[] = {6, 10, 9, 6, 6, 7, 7, 9, 4, 5, 3, 9, 5, 1};
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
  case sm_string_type:
    return sm_string_sprint((sm_string *)obj1, buffer, fake);
  case sm_expr_type:
    return sm_expr_sprint((sm_expr *)obj1, buffer, fake);
  case sm_meta_type:
    return sm_meta_sprint((sm_meta *)obj1, buffer, fake);
  case sm_context_type:
    return sm_context_sprint((sm_context *)obj1, buffer, fake);
  case sm_symbol_type:
    return sm_symbol_sprint((sm_symbol *)obj1, buffer, fake);
  case sm_fun_type:
    return sm_fun_sprint((sm_fun *)obj1, buffer, fake);
  case sm_local_type:
    return sm_local_sprint((sm_local *)obj1, buffer, fake);
  case sm_error_type:
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
  case sm_string_type:
    return sm_round_size(sizeof(sm_string) + ((sm_string *)obj1)->size + 1);
  case sm_symbol_type:
    return sizeof(sm_symbol);
  case sm_context_type:
    return sizeof(sm_context) + sizeof(sm_context_entry) * ((sm_context *)obj1)->capacity;
  case sm_pointer_type:
    return sizeof(sm_pointer);
  case sm_meta_type:
    return sizeof(sm_meta);
  case sm_fun_type:
    return sizeof(sm_fun) + sizeof(sm_fun_param) * ((sm_fun *)obj1)->num_params;
  case sm_fun_param_type:
    return sizeof(sm_fun_param_obj);
  case sm_local_type:
    return sizeof(sm_local);
  case sm_space_type:
    return ((sm_space *)obj1)->size;
  case sm_error_type:
    return sizeof(sm_error);
  default:
    printf("Cannot determine size of object of type %d\n", obj1->my_type);
    exit(0);
  }
}

bool sm_object_eq(sm_object *self, sm_object *other) {
  if (self->my_type != other->my_type)
    return false;
  switch (self->my_type) {
  case sm_double_type: {
    double value1 = ((sm_double *)self)->value;
    double value2 = ((sm_double *)other)->value;
    if (value1 == value2)
      return true;
    else
      return false;
  }
  case sm_string_type:
    return strcmp(&((sm_string *)self)->content, &((sm_string *)other)->content) == 0;
  case sm_symbol_type:
    return strcmp(&(((sm_symbol *)self)->name->content), &(((sm_symbol *)other)->name->content)) ==
           0;
  case sm_expr_type: {
    sm_expr *self_expr  = (sm_expr *)self;
    sm_expr *other_expr = (sm_expr *)other;
    if (self_expr->op != other_expr->op || self_expr->size != other_expr->size)
      return false;
    for (unsigned int i = 0; i < self_expr->size; i++) {
      if (!sm_object_eq(sm_expr_get_arg(self_expr, i), sm_expr_get_arg(other_expr, i)))
        return false;
    }
    return true;
  }
  case sm_primitive_type:
    return (void *)self == (void *)other;
  default:
    return false;
  }
}

// prints the object by using a buffer allocated from OS
void sm_object_dbg_print(sm_object *obj) {
  int   len = sm_object_sprint(obj, NULL, true);
  char *buf = malloc(len + 1);
  sm_object_sprint(obj, buf, false);
  buf[len] = '\0';
  printf("%s\n", buf);
  free(buf);
}
