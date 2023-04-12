// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

// understood to have array of num_param sm_fun_param following this structure
typedef struct sm_fun {
  short int          my_type;
  struct sm_context *parent;
  unsigned short int num_params;
  struct sm_object  *content;
} sm_fun;

// describes a parameter for a function
typedef struct sm_fun_param {
  struct sm_string *name;
  struct sm_object *default_val;
  short int         known_expr;
  short int         filler1;
  short int         filler2;
} sm_fun_param;

// wrapper to make a resizeable sm_object
// used for parsing
typedef struct sm_fun_param_obj {
  unsigned short int my_type;
  struct sm_string  *name;
  struct sm_object  *default_val;
  short int          known_expr;
} sm_fun_param_obj;

sm_fun *sm_new_fun(struct sm_context *parent, unsigned short int num_params, sm_object *content);
unsigned int      sm_fun_sprint(sm_fun *self, char *buffer, bool fake);
sm_fun_param_obj *sm_new_fun_param_obj(sm_string *name, sm_object *default_val,
                                       enum sm_object_type known_expr);
sm_fun_param     *sm_fun_get_param(sm_fun *self, unsigned short int i);
void sm_fun_set_param(sm_fun *self, unsigned short int i, sm_string *name, sm_object *default_val,
                      short int known_expr);
