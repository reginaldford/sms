// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

/// A boxed double-precision floating point value
typedef struct sm_i64 {
  uint32_t my_type;
  i64      value;
} sm_i64;

/// Create a new number
sm_i64 *sm_new_i64(i64 value);
/// Create a new number at a certain heap
sm_i64 *sm_new_i64_at(sm_heap *h, i64 value);
/// Convert the i64 to a string
sm_string *sm_i64_to_string(sm_i64 *self);
/// If !fake, print the i64 to a string buffer. Return length regardlessly
uint32_t sm_i64_sprint(sm_i64 *self, char *buffer, bool fake);
