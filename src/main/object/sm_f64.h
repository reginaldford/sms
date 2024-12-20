// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

/// A boxed double-precision floating point value
typedef struct sm_f64 {
  uint32_t my_type;
  f64      value;
} sm_f64;

/// Create a new number
sm_f64 *sm_new_f64(f64 value);
/// Create a new number at a certain heap
sm_f64 *sm_new_f64_at(sm_heap *h, f64 value);
/// Convert the f64 to a string
sm_string *sm_f64_to_string(sm_f64 *self);
/// If !fake, print the f64 to a string buffer. Return length regardlessly
uint32_t sm_f64_sprint(sm_f64 *self, char *buffer, bool fake);
