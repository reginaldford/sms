typedef struct sm_double {
  enum object_type my_type;
  double           value;
} sm_double;
sm_double *sm_new_double(double value);
