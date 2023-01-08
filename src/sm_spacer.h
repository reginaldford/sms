// This project is licensed under the BSD 2 clause license. See LICENSE.txt for more information.

typedef struct sm_spacer {
  enum sm_object_type my_type;
  unsigned int        size;
} sm_spacer;

sm_spacer *sm_new_spacer(unsigned int size);
