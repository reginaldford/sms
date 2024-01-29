// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

/// If there is only 1 value under a node of the tree, the node can point to a shortcut object.
/// Shortcut object replaces the nodes in a cx_node tree and stores the contained value.
typedef struct sm_shortcut {
  int16_t    my_type;
  sm_string *key;
  sm_object *value;
} sm_shortcut;

/// Create a new shortcut.
sm_shortcut *sm_new_shortcut(sm_object *value);
/// Returns the shortcut name
// sm_string *sm_shortcut_to_string(sm_shortcut *self);
/// Print the shortcut name to string buffer
// uint32_t sm_shortcut_sprint(sm_shortcut *self, char *buffer, bool fake);
