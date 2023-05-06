// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

// Contexts are data trees that map symbol names to values
typedef struct sm_cx {
  short int     my_type;
  struct sm_cx *parent;
  sm_node      *content;
} sm_cx;

sm_cx       *sm_new_cx(sm_cx *parent);
sm_object   *sm_cx_get(sm_cx *self, char *needle, int len);
sm_object   *sm_cx_get_far(sm_cx *self, char *needle, int len);
sm_cx       *sm_cx_get_container(sm_cx *self, char *needle, int len);
bool         sm_cx_let(sm_cx *context, char *needle, int len, sm_object *val);
bool         sm_cx_set(sm_cx *context, char *needle, int len, sm_object *val);
bool         sm_cx_rm(sm_cx *self, char *needle, int len);
unsigned int sm_cx_sprint(sm_cx *self, char *buffer, bool fake);

sm_node     *sm_new_cx_node(sm_object *value);
int          sm_node_index(char c);
bool         sm_node_is_empty(sm_node *node);
int          sm_node_map_size(unsigned long long map);
void         sm_node_map_set(unsigned long long *map, int index, bool on);
bool         sm_node_map_get(unsigned long long map, int i);
int          sm_node_child_index(unsigned long long map, int map_index);
void         sm_cx_clear(sm_cx *self);
unsigned int sm_cx_size(sm_cx *self);
