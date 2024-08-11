// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"

sm_array *sm_new_array(uint16_t type, uint32_t size, sm_object *content, uint32_t offset) {
  sm_array *output   = sm_malloc(sizeof(sm_array));
  output->my_type    = SM_ARRAY_TYPE;
  output->inner_type = type;
  output->size       = size;
  output->content    = content;
  return output;
}

double sm_f64_array_get_bare(sm_array *a, uint32_t index) {
  if (index >= a->size || a->inner_type != SM_F64_TYPE) {
    return 0.0; // or any other appropriate error value
  }
  return ((f64 *)&a->content[1])[index];
}

ui8 sm_ui8_array_get_bare(sm_array *a, uint32_t index) {
  if (index >= a->size || a->inner_type != SM_UI8_TYPE) {
    return 0; // or any other appropriate error value
  }
  return ((ui8 *)&a->content[1])[index];
}

// Print to a cstring buffer the description of array
// Return the resulting length
uint32_t sm_array_sprint(sm_array *a, char *buffer, bool fake) {
  uint32_t len = 0;
  char    *part1;
  char     f64_header[] = "f64[";
  char     ui8_header[] = "ui8[";
  switch (a->inner_type) {
  case SM_F64_TYPE:
    part1 = f64_header;
    len += 4;
    break;
  case SM_UI8_TYPE:
    part1 = ui8_header;
    len += 4;
    break;
  }
  if (!fake)
    for (uint8_t i = 0; i < len; i++)
      buffer[i] = part1[i];
  len += sm_array_contents_sprint(a, &(buffer[1]), fake);
  if (!fake)
    buffer[len] = ']';
  return ++len;
}

sm_object *sm_array_get(sm_array *a, uint32_t index) {
  switch (a->inner_type) {
  case SM_F64_TYPE: {
    // this number comes out wrong
    // double num = ((f64 *)&a->content[1])[index];
    double num = ((f64 *)&(a->content[1]))[index];
    return (sm_object *)sm_new_f64(num);
  }
  case SM_UI8_TYPE: {
    ui8 num = ((ui8 *)&a->content[1])[index];
    return (sm_object *)sm_new_ui8(num);
  }
  }
  return (sm_object *)sms_false;
}


sm_object *sm_f64_array_set(sm_array *a, uint32_t index, sm_f64 *number) {
  if (index >= a->size)
    return (sm_object *)sms_false;
  if (a->inner_type != SM_F64_TYPE)
    return (sm_object *)sms_false;
  ((f64 *)&a->content[1])[index] = number->value;
  return (sm_object *)sms_true;
}

sm_object *sm_ui8_array_set(sm_array *a, uint32_t index, sm_ui8 *number) {
  if (index >= a->size)
    return (sm_object *)sms_false;
  if (a->inner_type != SM_UI8_TYPE)
    return (sm_object *)sms_false;
  ((ui8 *)&a->content[1])[index] = number->value;
  return (sm_object *)sms_true;
}

uint32_t sm_array_contents_sprint(sm_array *array, char *buffer, bool fake) {
  if (array->size == 0)
    return 0;
  uint32_t cursor = 0;
  uint32_t i      = 0;
  for (; i + 1 < array->size; i++) {
    cursor += sm_object_sprint(sm_array_get(array, i), &(buffer[cursor]), fake);
    if (!fake)
      buffer[cursor] = ',';
    cursor++;
  }
  cursor += sm_object_sprint(sm_array_get(array, array->size - 1), &(buffer[cursor]), fake);
  return cursor;
}
