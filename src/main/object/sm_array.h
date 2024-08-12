// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

typedef struct sm_array {
  uint32_t   my_type;
  uint32_t   inner_type;
  uint32_t   size;
  uint32_t   offset;
  sm_object *content;
} sm_array;

sm_array  *sm_new_array(uint32_t type, uint32_t size, sm_object *content, uint32_t offset);
uint32_t   sm_array_sprint(sm_array *a, char *buffer, bool fake);
uint32_t   sm_array_contents_sprint(sm_array *a, char *buffer, bool fake);
sm_object *sm_f64_array_set(sm_array *a, uint32_t index, sm_f64 *number);
sm_object *sm_ui8_array_set(sm_array *a, uint32_t index, sm_ui8 *number);
f64        sm_f64_array_get_bare(sm_array *a, uint32_t index);
ui8        sm_ui8_array_get_bare(sm_array *a, uint32_t index);
