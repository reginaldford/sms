// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

/// A boxed double-precision floating point value
/// @note Until SMS has a type system, this is the only numeric type
typedef struct sm_double {
  short int my_type;
  double    value;
} sm_double;

/// Create a new number
sm_double *sm_new_double(double value);
/// Convert the double to a string
sm_string *sm_double_to_string(sm_double *self);
/// If !fake, print the double to a string buffer. Return length regardlessly
unsigned int sm_double_sprint(sm_double *self, char *buffer, bool fake);
