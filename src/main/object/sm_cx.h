// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

// These nodes branch from sm_cx
// 2 * 26 letters + underscore, single quote
// 54 total
typedef struct sm_cx_node {
  short int          my_type;
  struct sm_cx_node *child[64];
  struct sm_object  *value;
} sm_cx_node;

// Contexts are data trees that map symbol names to values
typedef struct sm_cx {
  short int     my_type;
  struct sm_cx *parent;
  sm_cx_node    content;
} sm_cx;

sm_cx       *sm_new_cx(sm_cx *parent);
int          sm_cx_node_index(char c);
sm_object   *sm_cx_get(sm_cx *self, char *needle, int len);
sm_object   *sm_cx_get_far(sm_cx *self, char *needle, int len);
bool         sm_cx_let(sm_cx *context, char *needle, int len, sm_object *val);
bool         sm_cx_set(sm_cx *context, char *needle, int len, sm_object *val);
sm_cx       *sm_cx_get_container(sm_cx *self, char *needle, int len);
bool         sm_cx_rm(sm_cx *self, char *needle, int len);
sm_cx_node  *sm_new_cx_node(sm_object *value);
bool         sm_cx_node_is_empty(sm_cx_node *node);
int          sm_find_last_index(sm_cx_node **arr, sm_cx_node *node);
unsigned int sm_cx_sprint(sm_cx *self, char *buffer, bool fake);
