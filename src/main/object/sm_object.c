// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"

extern sm_heap *sms_symbol_heap;
extern sm_heap *sms_symbol_name_heap;

// Whether an object is an integer (eventually, 8, 16, 32, or 64 bit)
// Returns 0 if it's not an int, and returns the number of bytes in the int otherwise
uint32_t sm_object_is_int(sm_object *obj1) {
  switch (obj1->my_type) {
  case SM_F64_TYPE:
  case SM_UI8_TYPE:
    return sm_sizeof(obj1);
  default:
    return 0;
  }
}

// Return a new sm_string describing the object
sm_string *sm_object_to_string(sm_object *obj1) {
  sm_string *new_str       = sm_new_string_manual(sm_object_sprint(obj1, NULL, true));
  uint32_t   len           = sm_object_sprint(obj1, &(new_str->content), false);
  (&new_str->content)[len] = '\0';
  return new_str;
}

// Print to a string buffer the object description
// Return the length of the string
// @note This function is called twice: once to precalculate buffer size,
// then, again, with an allocated buffer of that size. Functions called from here can use memory,
// but sms_heap->used will be reset to the value it had when this functoin is called.
uint32_t sm_object_sprint(sm_object *obj1, char *buffer, bool fake) {
  uint32_t len        = 0;
  uint32_t used_space = sms_heap->used;
  switch (obj1->my_type) {
  case SM_F64_TYPE:
    len += sm_f64_sprint((sm_f64 *)obj1, buffer, fake);
    break;
  case SM_STRING_TYPE:
    len += sm_string_sprint((sm_string *)obj1, buffer, fake);
    break;
  case SM_EXPR_TYPE:
    len += sm_expr_sprint((sm_expr *)obj1, buffer, fake);
    break;
  case SM_META_TYPE:
    len += sm_meta_sprint((sm_meta *)obj1, buffer, fake);
    break;
  case SM_CX_TYPE:
    len += sm_cx_sprint((sm_cx *)obj1, buffer, fake);
    break;
  case SM_SYMBOL_TYPE:
    len += sm_symbol_sprint((sm_symbol *)obj1, buffer, fake);
    break;
  case SM_FUN_TYPE:
    len += sm_fun_sprint((sm_fun *)obj1, buffer, fake);
    break;
  case SM_LOCAL_TYPE:
    len += sm_local_sprint((sm_local *)obj1, buffer, fake);
    break;
  case SM_ERR_TYPE:
    len += sm_err_sprint((sm_error *)obj1, buffer, fake);
    break;
  case SM_RETURN_TYPE:
    len += sm_return_sprint((sm_return *)obj1, buffer, fake);
    break;
  case SM_SELF_TYPE:
    len += sm_self_sprint((sm_self *)obj1, buffer, fake);
    break;
  case SM_ARRAY_TYPE:
    len += sm_array_sprint((sm_array *)obj1, buffer, fake);
    break;
  case SM_UI8_TYPE:
    len += sm_ui8_sprint((sm_ui8 *)obj1, buffer, fake);
    break;
  default: {
    if (!fake)
      len += sprintf(buffer, "?(%i)", obj1->my_type);
    else {
      char fake_buffer[10];
      len += sprintf(fake_buffer, "?(%i)", obj1->my_type);
    }
  }
  }
  sms_heap->used = used_space;
  return len;
}
// Return the size of the object in bytes

uint32_t sm_sizeof(sm_object *obj1) {
  switch (obj1->my_type) {
  case SM_F64_TYPE:
    return sizeof(sm_f64);
  case SM_EXPR_TYPE:
    return sizeof(sm_expr) + sizeof(sm_object *) * ((sm_expr *)obj1)->capacity;
  case SM_PRIMITIVE_TYPE:
    return sizeof(sm_expr);
  case SM_STRING_TYPE:
    return sm_round_size64(sizeof(sm_string) - 3 + ((sm_string *)obj1)->size);
  case SM_SYMBOL_TYPE:
    return sizeof(sm_symbol);
  case SM_CX_TYPE:
    return sizeof(sm_cx);
  case SM_NODE_TYPE:
    return sizeof(sm_node);
  case SM_POINTER_TYPE:
    return sizeof(sm_pointer);
  case SM_META_TYPE:
    return sizeof(sm_meta);
  case SM_FUN_TYPE:
    return sizeof(sm_fun) + sizeof(sm_fun_param) * ((sm_fun *)obj1)->num_params;
  case SM_FUN_PARAM_TYPE:
    return sizeof(sm_fun_param_obj);
  case SM_LOCAL_TYPE:
    return sizeof(sm_local);
  case SM_SPACE_TYPE:
    return sizeof(sm_space) + sm_round_size64(((sm_space *)obj1)->size);
  case SM_SELF_TYPE:
    return sizeof(struct sm_self);
  case SM_ERR_TYPE:
    return sizeof(sm_error);
  case SM_RETURN_TYPE:
    return sizeof(sm_return);
  case SM_STACK_OBJ_TYPE:
    return sizeof(sm_stack_obj) + sizeof(void *) * ((sm_stack_obj *)obj1)->capacity;
  case SM_ARRAY_TYPE:
    return sizeof(sm_array);
  case SM_UI8_TYPE:
    return sizeof(sm_ui8);
  default:
    return 0;
  }
}

// Recursive equality checking.
// In general, If two objects have the same toString, this should return true.
// But this is much faster than toStr(obj1)==toStr(obj2)
bool sm_object_eq(sm_object *self, sm_object *other) {
  if (self->my_type != other->my_type)
    return false;
  switch (self->my_type) {
  case SM_F64_TYPE: {
    f64 value1 = ((sm_f64 *)self)->value;
    f64 value2 = ((sm_f64 *)other)->value;
    if (value1 == value2)
      return true;
    else
      return false;
  }
  case SM_UI8_TYPE: {
    ui8 value1 = ((sm_ui8 *)self)->value;
    ui8 value2 = ((sm_ui8 *)other)->value;
    if (value1 == value2)
      return true;
    else
      return false;
  }
  case SM_STRING_TYPE:
    return strcmp(&((sm_string *)self)->content, &((sm_string *)other)->content) == 0;
  case SM_SYMBOL_TYPE:
    return strcmp(&(((sm_symbol *)self)->name->content), &(((sm_symbol *)other)->name->content)) ==
           0;
  case SM_EXPR_TYPE: {
    sm_expr *self_expr  = (sm_expr *)self;
    sm_expr *other_expr = (sm_expr *)other;
    if (self_expr->op != other_expr->op || self_expr->size != other_expr->size)
      return false;
    for (uint32_t i = 0; i < self_expr->size; i++) {
      if (!sm_object_eq(sm_expr_get_arg(self_expr, i), sm_expr_get_arg(other_expr, i)))
        return false;
    }
    return true;
  }
  case SM_PRIMITIVE_TYPE:
    return (void *)self == (void *)other;
  case SM_CX_TYPE: {
    sm_cx *self_cx    = (sm_cx *)self;
    sm_cx *other_cx   = (sm_cx *)other;
    int    my_size    = sm_cx_size(self_cx);
    int    their_size = sm_cx_size(other_cx);
    if (my_size != their_size)
      return false;
    // TODO: create iterators, to avoid allocating heap memory just to compute equality
    sm_expr *my_keys = sm_node_keys(self_cx->content, sm_new_stack_obj(32),
                                    sm_new_expr_n(SM_TUPLE_EXPR, 0, my_size, NULL));
    sm_expr *my_values =
      sm_node_values(self_cx->content, sm_new_expr_n(SM_TUPLE_EXPR, 0, my_size, NULL));
    sm_expr *their_keys = sm_node_keys(other_cx->content, sm_new_stack_obj(32),
                                       sm_new_expr_n(SM_TUPLE_EXPR, 0, their_size, NULL));
    sm_expr *their_values =
      sm_node_values(other_cx->content, sm_new_expr_n(SM_TUPLE_EXPR, 0, their_size, NULL));
    for (int i = 0; i < my_size; i++) {
      if (!sm_object_eq(sm_expr_get_arg(my_keys, i), sm_expr_get_arg(their_keys, i)))
        return false;
      if (!sm_object_eq(sm_expr_get_arg(my_values, i), sm_expr_get_arg(their_values, i)))
        return false;
    }
    return true;
  }
  default:
    return self == other;
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
