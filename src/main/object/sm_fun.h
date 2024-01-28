// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

/// An sm_fun struct is understood to have an array of sm_fun_param structs following.
/// The number of sm_fun_param structs that follow is specified in num_params
typedef struct sm_fun {
  int16_t           my_type;
  struct sm_cx     *parent;
  uint16_t          num_params;
  struct sm_object *content;
} sm_fun;

/// Describes a parameter for a function
typedef struct sm_fun_param {
  struct sm_string *name;
  struct sm_object *default_val;
  int16_t           known_expr;
} sm_fun_param;

/// This is an independant garbage-collectable object version of sm_fun_param
/// This type of object is used for parsing.
typedef struct sm_fun_param_obj {
  uint16_t          my_type;
  struct sm_string *name;
  struct sm_object *default_val;
  int16_t           known_expr;
} sm_fun_param_obj;

/// Create a new function
sm_fun *sm_new_fun(struct sm_cx *parent, uint16_t num_params, sm_object *content);
/// Print function to string
uint32_t sm_fun_sprint(sm_fun *self, char *buffer, bool fake);
/// Create a new function parameter object
sm_fun_param_obj *sm_new_fun_param_obj(sm_string *name, sm_object *default_val,
                                       enum sm_object_type known_expr);
/// Get a function parameter (not object) from a function
sm_fun_param *sm_fun_get_param(sm_fun *self, uint16_t i);
/// Set the ith function parameter on a function
void sm_fun_set_param(sm_fun *self, uint16_t i, sm_string *name, sm_object *default_val,
                      int16_t known_expr);
