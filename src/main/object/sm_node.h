// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

// These nodes branch from sm_cx
typedef struct sm_node {
  short int          my_type;  // SM_NODE_TYPE
  struct sm_object  *value;    // NULL if nothing
  struct sm_node    *next;     // Like changing this letter
  unsigned long long map;      // 1's for existing children
  struct sm_node    *children; // Like moving cursor over
} sm_node;

sm_node        *sm_new_node(sm_object *value, struct sm_node *next, long long map,
                            struct sm_node *children);
sm_object      *sm_node_get(sm_node *self, char *needle, int len);
sm_node        *sm_node_get_leaf(sm_node *root, char *needle, int len);
void            sm_node_set(sm_node *node, int which, sm_object *value);
bool            sm_node_is_empty(sm_node *node);
int             sm_node_map_index(char c);
char            sm_node_bit_unindex(int i);
bool            sm_node_insert(struct sm_node *root, struct sm_node *new_node, int where);
sm_node        *sm_node_nth(sm_node *self, int index);
sm_node        *sm_node_subnode(sm_node *self, char *needle, int len);
sm_node        *sm_node_get_container(sm_node *self, char *needle, int len);
int             sm_node_sprint(sm_node *node, char *buffer, bool fake, sm_stack *char_stack);
sm_node        *sm_node_parent_node(sm_node *self, char *needle, int len);
struct sm_node *sm_node_rm(struct sm_node *root, struct sm_node *nodeToRemove);
sm_node        *sm_node_unique_root(struct sm_node *root, char *needle, int len);
int             sm_node_map_left_count(unsigned long long map, int map_index);
bool            sm_node_rm_nth(struct sm_node *root, int n);
int             sm_node_size(sm_node *node);
sm_expr        *sm_node_keys(sm_node *node, sm_stack *char_stack, sm_expr *collection);
sm_expr        *sm_node_values(sm_node *node, sm_expr *collection);
int             sm_node_index(char c);
bool            sm_node_is_empty(sm_node *node);
int             sm_node_map_size(unsigned long long map);
void            sm_node_map_set(unsigned long long *map, int index, bool on);
bool            sm_node_map_get(unsigned long long map, int i);
int             sm_node_child_index(unsigned long long map, int map_index);
