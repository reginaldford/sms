// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information
#include <stdint.h>

// These nodes branch from sm_cx
typedef struct sm_cx_node {
  short int          my_type;
  struct sm_object  *value;
  unsigned long long map;
  struct sm_link    *children;
} sm_cx_node;

// Contexts are data trees that map symbol names to values
typedef struct sm_cx {
  short int     my_type;
  struct sm_cx *parent;
  sm_cx_node   *content;
} sm_cx;

sm_cx       *sm_new_cx(sm_cx *parent);
sm_object   *sm_cx_get(sm_cx *self, char *needle, int len);
sm_object   *sm_cx_get_far(sm_cx *self, char *needle, int len);
sm_cx       *sm_cx_get_container(sm_cx *self, char *needle, int len);
bool         sm_cx_let(sm_cx *context, char *needle, int len, sm_object *val);
bool         sm_cx_set(sm_cx *context, char *needle, int len, sm_object *val);
bool         sm_cx_rm(sm_cx *self, char *needle, int len);
unsigned int sm_cx_sprint(sm_cx *self, char *buffer, bool fake);

sm_cx_node *sm_new_cx_node(sm_object *value);
int         sm_cx_node_index(char c);
bool        sm_cx_node_is_empty(sm_cx_node *node);
int         sm_cx_node_map_size(unsigned long long map);
void        sm_cx_node_map_set(unsigned long long *map, int index, bool on);
bool        sm_cx_node_map_get(unsigned long long map, int i);
int         sm_cx_node_child_index(unsigned long long map, int bit_index);
