// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

typedef struct sm_array {
  uint32_t   my_type;
  uint32_t   inner_type;
  uint32_t   size;
  uint32_t   offset;
  sm_object *content;
} sm_array;

sm_array  *sm_new_array(uint32_t type, uint32_t size, sm_object *content, uint32_t offset);
double    *sm_f64_array_get_start(sm_array *a);
uint32_t   sm_array_sprint(sm_array *a, char *buffer, bool fake);
uint32_t   sm_array_contents_sprint(sm_array *a, char *buffer, bool fake);
void       sm_f64_array_set(sm_array *a, uint32_t index, double number);
void       sm_ui8_array_set(sm_array *a, uint32_t index, uint8_t number);
double     sm_f64_array_get_bare(sm_array *a, uint32_t index);
uint8_t   *sm_ui8_array_get_start(sm_array *a);
uint8_t    sm_ui8_array_get_bare(sm_array *a, uint32_t index);
sm_object *sm_array_get(sm_array *a, uint32_t index);
