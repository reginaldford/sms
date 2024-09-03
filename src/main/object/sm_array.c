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

f64 *sm_f64_array_get_start(sm_array *a) { return (f64 *)(&((sm_space *)a->content)[1]); }

f64 sm_f64_array_get_bare(sm_array *a, uint32_t index) { return sm_f64_array_get_start(a)[index]; }

ui8 *sm_ui8_array_get_start(sm_array *a) { return (ui8 *)(&((sm_space *)a->content)[1]); }

ui8 sm_ui8_array_get_bare(sm_array *a, uint32_t index) { return sm_ui8_array_get_start(a)[index]; }

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

void sm_f64_array_set(sm_array *a, uint32_t index, f64 number) {
  ((f64 *)sm_f64_array_get_start(a))[index] = number;
}

void sm_ui8_array_set(sm_array *a, uint32_t index, ui8 number) {
  ((ui8 *)sm_ui8_array_get_start(a))[index] = number;
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
  uint32_t cursor = 0;
  uint32_t i      = 0;

  for (i = 0; i < array->size; i++) {
    f64 value = sm_f64_array_get_bare(array, i);
    // Estimate maximum buffer size needed for one float
    char num_buf[36]; // Enough to hold any formatted float
    int  len = snprintf(num_buf, sizeof(num_buf), "%.16g", value);

    if (!fake)
      if (len > 0)
        memcpy(&buffer[cursor], num_buf, len); // Copy the formatted string to the buffer
    cursor += len;
    // Add a comma if it's not the last element
    if (i + 1 < array->size) {
      if (!fake)
        buffer[cursor] = ',';
      cursor++;
    }
  }

  // Null-terminate the buffer if not faking
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
  for (i = 0; i < array->size; i++) {
    uint8_t value = sm_ui8_array_get_bare(array, i);
    // Calculate the length needed to print the integer
    uint32_t len = (value == 0) ? 1 : (uint32_t)log10(value) + 1;
    if (!fake)
      cursor += sprintf(&buffer[cursor], "%u", value);
    else
      cursor += len; // Only increment cursor by the length if faking

    // Add a comma if it's not the last element
    if (i + 1 < array->size) {
      if (!fake)
        buffer[cursor] = ',';
      cursor++;
    }
  }
  // Null-terminate the buffer if not faking
  if (!fake)
    buffer[cursor] = '\0';
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
