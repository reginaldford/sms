// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"

sm_array *sm_new_array(uint16_t type, uint32_t size) {
  sm_array *output =
    sm_malloc(sizeof(sm_array) + (sizeof(type) - sm_type_header_size(type)) * size);
  output->my_type    = SM_ARRAY_TYPE;
  output->inner_type = type;
  output->size       = size;
  return output;
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
  // else if(a->inner_type==SM_U8_TYPE)
  if (!fake)
    for (uint8_t i = 0; i < 4; i++)
      buffer[i] = part1[i];
  len += sm_array_contents_sprint(a, &(buffer[1]), fake);
  if (!fake)
    buffer[len] = ']';
  return ++len;
}

sm_object *sm_array_get_arg(sm_array *a, uint32_t index) {
  switch (a->inner_type) {
  case SM_F64_TYPE:
    return (sm_object *)(&a->content)[index];
  }
  return (sm_object *)sms_false;
}

uint32_t sm_array_contents_sprint(sm_array *array, char *buffer, bool fake) {
  if (array->size == 0)
    return 0;
  uint32_t cursor = 0;
  uint32_t i      = 0;
  for (; i + 1 < array->size; i++) {
    cursor += sm_object_sprint(sm_array_get_arg(array, i), &(buffer[cursor]), fake);
    if (!fake)
      buffer[cursor] = ',';
    cursor++;
  }
  if (array->size > 0)
    cursor += sm_object_sprint(sm_array_get_arg(array, array->size - 1), &(buffer[cursor]), fake);
  return cursor;
}
