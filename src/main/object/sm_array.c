// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"

sm_array *sm_new_array(uint32_t type, uint32_t size, sm_object *content, uint32_t offset) {
  sm_array *output   = sm_malloc(sizeof(sm_array));
  output->my_type    = SM_ARRAY_TYPE;
  output->inner_type = type;
  output->size       = size;
  output->offset     = offset;
  output->content    = content;
  return output;
}


f64 sm_f64_array_get_bare(sm_array *a, uint32_t index) {
  // if (index >= a->size || a->inner_type != SM_F64_TYPE) {
  return sm_f64_array_get_start(a)[index];
}


f64 *sm_f64_array_get_start(sm_array *a) { return (f64 *)(&((sm_space *)a->content)[1]); }
ui8 *sm_ui8_array_get_start(sm_array *a) { return (ui8 *)(&((sm_space *)a->content)[1]); }

// f64 *sm_f64_array_get_start(sm_array *a) { return (f64 *)(((char*)a->content)+a->offset); }
// ui8 *sm_ui8_array_get_start(sm_array *a) { return (ui8 *)(((char*)a->content)+a->offset); }

ui8 sm_ui8_array_get_bare(sm_array *a, uint32_t index) {
  // if (index >= a->size || a->inner_type != SM_UI8_TYPE) {
  return sm_ui8_array_get_start(a)[index];
}

sm_object *sm_array_get(sm_array *a, uint32_t index) {
  switch (a->inner_type) {
  case SM_F64_TYPE: {
    return (sm_object *)sm_new_f64(sm_f64_array_get_bare(a, index));
  }
  case SM_UI8_TYPE: {
    return (sm_object *)sm_new_ui8(sm_ui8_array_get_bare(a, index));
  }
  }
  return (sm_object *)sms_false;
}

sm_object *sm_f64_array_set(sm_array *a, uint32_t index, f64 number) {
  if (index >= a->size)
    return (sm_object *)sms_false;
  ((f64 *)sm_f64_array_get_start(a))[index] = number;
  return (sm_object *)sms_true;
}

sm_object *sm_ui8_array_set(sm_array *a, uint32_t index, ui8 number) {
  if (index >= a->size)
    return (sm_object *)sms_false;
  ((char *)sm_ui8_array_get_start(a))[index] = number;
  return (sm_object *)sms_true;
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
  len += sm_array_contents_sprint(a, &(buffer[4]), fake);
  if (!fake)
    buffer[len] = ']';
  return ++len;
}

uint32_t sm_f64_array_contents_sprint(sm_array *array, char *buffer, bool fake) {
  uint32_t cursor       = 0;
  uint32_t bufferCursor = 0;
  uint32_t i            = 0;
  char     fakeBuf[25];
  if (fake) {
    buffer = fakeBuf;
  }
  for (i = 0; i < array->size; i++) {
    if (fake) {
      bufferCursor = 0; // Reset bufferCursor to 0 if we are faking
    }
    f64 value    = sm_f64_array_get_bare(array, i);
    bufferCursor = snprintf(&buffer[cursor], 24, "%.16g", value);
    cursor += bufferCursor;
    if (i + 1 < array->size) {
      if (!fake) {
        buffer[cursor] = ',';
      }
      cursor++;
    }
  }
  if (!fake) {
    buffer[cursor] = '\0';
  }
  return cursor;
}

uint32_t sm_ui8_array_contents_sprint(sm_array *array, char *buffer, bool fake) {
  if (array->size == 0)
    return 0;
  uint32_t cursor = 0;
  uint32_t i      = 0;
  char     tmp[20];
  if (fake)
    buffer = tmp;
  for (; i + 1 < array->size; i++) {
    cursor += sprintf(&buffer[cursor], "%i", sm_ui8_array_get_bare(array, i));
    if (!fake)
      buffer[cursor] = ',';
    cursor++;
  }
  if (array->size > 0) {
    cursor += sprintf(&buffer[cursor], "%i", sm_ui8_array_get_bare(array, array->size - 1));
  }
  return cursor;
}

uint32_t sm_array_contents_sprint(sm_array *a, char *buffer, bool fake) {
  uint32_t cursor = 0;
  switch (a->inner_type) {
  case SM_F64_TYPE: {
    return sm_f64_array_contents_sprint(a, buffer, fake);
  }
  case SM_UI8_TYPE: {
    return sm_ui8_array_contents_sprint(a, buffer, fake);
  }
  }
  return 0;
}
