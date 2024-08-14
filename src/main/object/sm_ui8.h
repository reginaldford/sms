// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

/// A boxed ui8-precision floating point value
typedef struct sm_ui8 {
  uint32_t my_type;
  uint8_t  value;
} sm_ui8;

/// Create a new number
sm_ui8 *sm_new_ui8(ui8 value);
/// Convert the ui8 to a string
sm_string *sm_ui8_to_string(sm_ui8 *self);
/// If !fake, print the ui8 to a string buffer. Return length regardlessly
uint32_t sm_ui8_sprint(sm_ui8 *self, char *buffer, bool fake);
