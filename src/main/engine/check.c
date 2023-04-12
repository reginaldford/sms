#include "../sms.h"

// the engine should return an error if this returns false
// and the next function will complain about type mismatch,
// going up the callstack, until we hit a try or not
// the checker is the type number of the function which is checking
bool expect_type(sm_object *arg_n, unsigned int arg_num, unsigned short int arg_type,
                 unsigned short int checker) {
  if (arg_n->my_type != arg_type) {
    sm_string  *str  = sm_object_to_string(arg_n);
    const char *cstr = &(str->content);
    printf("Error: %s requires argument %i to have type %s but %s has type %s\n",
           sm_global_fn_name(checker), arg_num, sm_global_type_name(arg_type), cstr,
           sm_global_type_name(arg_n->my_type));
    return false;
  }
  return true;
}
