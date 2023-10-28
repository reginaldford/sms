// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

/// These mapped trie nodes make up trees rooted in sm_cx structs
typedef struct sm_node {
  /// The type of this object (SM_NODE_TYPE)
  short int my_type;
  /// The value is null or some object
  struct sm_object *value;
  /// Go to the next sibling node
  struct sm_node *next;
  /// a 64 bit map with 1's for existing children of this node
  unsigned long long map;
  /// Children nodes form a linked list
  struct sm_node *children;
} sm_node;

/// Construct a new node
sm_node *sm_new_node(sm_object *value, struct sm_node *next, long long map,
                     struct sm_node *children);
/// Get the value inside the node addressed by needle
sm_object *sm_node_get(sm_node *self, char *needle, int len);
/// Get the node addressed by needle
sm_node *sm_node_get_leaf(sm_node *root, char *needle, int len);
/// Set the value of the leaf node addressed by value
void sm_node_set(sm_node *node, int which, sm_object *value);
/** @brief Return the sm_node child index correlating to this character
@param c A-Z,a-z,_, or single quote ("calculus prime symbol")
@note Char   ASCII
@note '   is 39
@note 0-9 is 48-57
@note A-Z is 65-90
@note _   is 95
@note a-z is 98-122
@note Total: 64 */
int sm_node_map_index(char c);
/** @brief Inverse of sm_map_index.
@return A letter , underscore, or single quote
@param i Integer from 0 to 63 **/
char sm_node_bit_unindex(int i);
/// Add a child node to root node (parent node)
/// @param where Specifies which character this node represents
bool sm_node_insert(struct sm_node *root, struct sm_node *new_node, int where);
/// Get the nth node in the linked list of cx_node stuctures
sm_node *sm_node_nth(sm_node *self, int index);
/// Get the node containing the value that this key (needle) addresses in the tree
sm_node *sm_node_subnode(sm_node *self, char *needle, int len);
/// Get the containing node (parent) of the node addressed by needle
sm_node *sm_node_get_container(sm_node *self, char *needle, int len);
/// If !fake, print this node into buffer. Return the string length regardlessly.
int sm_node_sprint(sm_node *node, char *buffer, bool fake, sm_stack *char_stack);
/// Get the parent node of this node. May be NULL.
sm_node *sm_node_parent_node(sm_node *self, char *needle, int len);
/// Remove a particular node from this tree
struct sm_node *sm_node_rm(struct sm_node *root, struct sm_node *nodeToRemove);
/// Return the number of set bits to the left of map_index'th bit in map
int sm_node_map_left_count(unsigned long long map, int map_index);
/// Remove the nth node in the linked list started at root
bool sm_node_rm_nth(struct sm_node *root, int n);
/// Returns the number of non-NULL values in nodes under this node (including this node)
int sm_node_size(sm_node *node);
/// Return an array of the keys (as string objects) under this node
sm_expr *sm_node_keys(sm_node *node, sm_stack *char_stack, sm_expr *collection);
/// Return an array of the values (as string objects) under this node
sm_expr *sm_node_values(sm_node *node, sm_expr *collection);
/// A node is empty if is has NULL as value and 0LL as map
bool sm_node_is_empty(sm_node *node);
/// Return the number of children to this node
int sm_node_map_size(unsigned long long map);
/// Set a bit of map to 1 or 0 depending on the provided boolean
void sm_node_map_set(unsigned long long *map, int index, bool on);
/// Return whether the bit at i is set in the map
bool sm_node_map_get(unsigned long long map, int i);
/// Return the correlating child index to this bit in the map
int sm_node_child_index(unsigned long long map, int map_index);
