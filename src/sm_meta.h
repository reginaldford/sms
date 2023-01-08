// This project is licensed under the BSD 2 clause license. See LICENSE.txt for more information.
typedef struct sm_meta {
  int        my_type;
  int        meta_level;
  sm_object *address;
} sm_meta;

sm_meta   *sm_new_meta(sm_object *address);
sm_string *sm_meta_to_string(sm_meta *meta);
