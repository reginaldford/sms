// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

/// A boxed double-precision floating point value
typedef struct sm_ui64 {
  uint32_t my_type;
  ui64     value;
} sm_ui64;

/// Create a new number
sm_ui64 *sm_new_ui64(ui64 value);
/// Create a new number at a certain heap
sm_ui64 *sm_new_ui64_at(sm_heap *h, ui64 value);
/// Convert the ui64 to a string
sm_string *sm_ui64_to_string(sm_ui64 *self);
/// If !fake, print the ui64 to a string buffer. Return length regardlessly
uint32_t sm_ui64_sprint(sm_ui64 *self, char *buffer, bool fake);
