// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "sms.h"

// new sm_fun structure
sm_fun *sm_new_fun(sm_context *parent, unsigned short int num_params, sm_object *content) {
  sm_fun *self     = sm_malloc(sizeof(sm_fun) + num_params * sizeof(sm_fun_param));
  self->my_type    = sm_fun_type;
  self->parent     = parent;
  self->num_params = num_params;
  self->content    = content;
  return self;
}

// new sm_fun_param structure
sm_fun_param_obj *sm_new_fun_param_obj(sm_string *name, sm_object *default_val,
                                       enum sm_object_type known_type) {
  sm_fun_param_obj *self = sm_malloc(sizeof(sm_fun));
  self->name             = name;
  self->default_val      = default_val;
  self->known_type       = known_type;
  return self;
}

// Return the i'th element of the array of sm_fun_param objects following the sm_fun struct
sm_fun_param *sm_fun_get_param(sm_fun *self, unsigned short int i) {
  sm_fun_param *arr = (sm_fun_param *)&(self[1]);
  return &(arr[i]);
}

// Set the i'th element of the array of sm_fun_param objects following the sm_fun struct
void sm_fun_set_param(sm_fun *self, unsigned short int i, sm_string *name, sm_object *default_val,
                      short int known_type) {
  sm_fun_param *arr  = (sm_fun_param *)&(self[1]);
  arr[i].name        = name;
  arr[i].default_val = default_val;
  arr[i].known_type  = known_type;
}

// Print to c str buffer a description of this parameter
int sm_fun_param_sprint(sm_fun_param *self, char *buffer) {
  sm_strncpy(buffer, &(self->name->content), self->name->size);
  unsigned int cursor = self->name->size;
  if (self->default_val != NULL) {
    buffer[cursor++] = '=';
    cursor += sm_object_sprint(self->default_val, &(buffer[cursor]));
  }
  return cursor;
}

// return value of hypothetical call to sm_fun_param_sprint
int sm_fun_param_to_string_len(sm_fun_param *self) {
  unsigned int cursor = self->name->size;
  if (self->default_val != NULL) {
    cursor++;
    cursor += sm_object_to_string_len(self->default_val);
  }
  return cursor;
}

//! support for calculation-only mode
// fun:add(a,b)=a+b;
unsigned int sm_fun_sprint(sm_fun *self, char *buffer) {
  buffer[0]           = '(';
  unsigned int cursor = 1;
  for (unsigned short int i = 0; i < self->num_params - 1; i++) {
    sm_fun_param *fp = sm_fun_get_param(self, i);
    cursor += sm_fun_param_sprint(fp, &(buffer[cursor]));
    buffer[cursor++] = ',';
  }
  sm_fun_param *fp = sm_fun_get_param(self, self->num_params - 1);
  cursor += sm_fun_param_sprint(fp, &(buffer[cursor]));
  buffer[cursor++] = ')';
  buffer[cursor++] = '=';
  buffer[cursor++] = '>';
  cursor += sm_object_sprint(self->content, &(buffer[cursor]));
  return cursor;
}

// Return value of hypothetical call to sm_fun_sprint
unsigned int sm_fun_to_string_len(sm_fun *self) {
  unsigned int cursor = 1;
  for (unsigned short int i = 0; i < self->num_params - 1; i++) {
    cursor += sm_fun_param_to_string_len(sm_fun_get_param(self, i));
    cursor++;
  }
  cursor += sm_fun_param_to_string_len(sm_fun_get_param(self, self->num_params - 1));
  cursor += 3;
  cursor += sm_object_to_string_len(self->content);
  return cursor;
}
