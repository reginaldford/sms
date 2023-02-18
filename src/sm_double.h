// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

typedef struct sm_double {
  short int my_type;
  double    value;
} sm_double;

sm_double   *sm_new_double(double value);
sm_string   *sm_double_to_string(sm_double *self);
unsigned int sm_double_sprint(sm_double *self, char *buffer, bool fake);
