typedef struct sm_double {
  enum sm_object_type my_type;
  double              value;
} sm_double;

sm_double *sm_new_double(double value);
sm_string *sm_double_to_string(sm_double *self);
