// This project is licensed under the BSD 2 clause license. See LICENSE.txt for more information.
typedef struct sm_double {
  enum sm_object_type my_type;
  double              value;
} sm_double;

sm_double *sm_new_double(double value);
sm_string *sm_double_to_string(sm_double *self);
