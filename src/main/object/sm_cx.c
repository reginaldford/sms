// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"

// Create a new context, an array of key_values sorted by key
sm_cx *sm_new_cx(sm_cx *parent) {
  sm_cx *new_cx   = (sm_cx *)sm_malloc(sizeof(sm_cx));
  new_cx->my_type = SM_CX_TYPE;
  new_cx->parent  = parent;
  new_cx->content = NULL;
  return new_cx;
}

// Search through this cx and ancestry for this key.
// Return the containing CX if the key is found, else return false.
sm_cx *sm_cx_get_container(sm_cx *self, char *needle, int len) {
  sm_cx *current_loc = self;
  while (current_loc) {
    sm_object *result = sm_node_get(current_loc->content, needle, len);
    if (result)
      return current_loc;
    current_loc = current_loc->parent;
  }
  return NULL;
}

// Searches for an existing value and sets it to the given value and returns True.
// Returns false if the value cannot be found. Traverses parent ptrs to find the value.
bool sm_cx_set(sm_cx *self, char *needle, int len, sm_object *value) {
  sm_cx *cx = self;
  while (cx != NULL) {
    sm_node *node = cx->content;
    sm_node *leaf = sm_node_subnode(node, needle, len);
    if (leaf != NULL) {
      leaf->value = value;
      return true;
    }
    cx = cx->parent;
  }
  return false;
}

// Get this value from the current context.
// Return Null if there is no such key.
sm_object *sm_cx_get(sm_cx *self, char *needle, int len) {
  sm_node *node = self->content;
  if (node != NULL) {
    sm_node *leaf = sm_node_subnode(node, needle, len);
    if (leaf != NULL)
      return leaf->value;
  }
  return NULL;
}

// Like sm_cx_get, but
// traverses the context ancestry to find the result, or returns NULL
sm_object *sm_cx_get_far(sm_cx *self, char *needle, int len) {
  sm_cx *curr_cx = self;
  while (curr_cx != NULL) {
    if (curr_cx->content != NULL) {
      sm_node *leaf = sm_node_subnode(curr_cx->content, needle, len);
      if (leaf != NULL)
        return leaf->value;
    }
    curr_cx = curr_cx->parent;
  }
  return NULL;
}

// Add a key_value with this key and value
bool sm_cx_let(sm_cx *self, char *needle, int len, sm_object *val) {
  sm_node *current_node;
  if (self->content == NULL)
    self->content = sm_new_node(NULL, NULL, 0LL, NULL);
  current_node = self->content;
  for (int i = 0; i < len; i++) {
    int             index       = sm_node_map_index(needle[i]);
    int             child_index = sm_node_child_index(current_node->map, index);
    struct sm_node *next_node;
    if (sm_node_map_get(current_node->map, index) == false) {
      sm_node_map_set(&current_node->map, index, true);
      next_node = sm_new_node(NULL, NULL, 0LL, NULL);
      if (child_index == 0) {
        next_node->next        = current_node->children;
        current_node->children = next_node;
      } else
        sm_node_insert(current_node->children, next_node, child_index);
    } else
      next_node = sm_node_nth(current_node->children, child_index);
    current_node = (sm_node *)next_node;
  }
  // Leaf node already exists with this key
  if (current_node->value != NULL)
    return false;
  current_node->value = (sm_object *)val;
  return true;
}

// Remove the node addressed by needle and return true.
// Return false if the node is not found.
bool sm_cx_rm(sm_cx *self, char *needle, int len) {
  sm_node *node        = sm_node_subnode(self->content, needle, len);
  sm_node *node_parent = sm_node_parent_node(self->content, needle, len);
  if (node != NULL && node->value != NULL) {
    int map_index = sm_node_map_index(needle[len - 1]);
    sm_node_map_set(&node_parent->map, map_index, false);
    node->value = NULL;
    return true;
  } else {
    return false;
  }
}

// Print the contents of this cx to buffer and return the length
// If fake is true, just return the hypotehtical string length
unsigned int sm_cx_sprint(sm_cx *self, char *buffer, bool fake) {
  if (!fake)
    buffer[0] = '{';
  int cursor = 1;
  if (self->content != NULL) {
    sm_stack *letter_stack = sm_new_stack(32);
    cursor += sm_node_sprint(self->content, &(buffer[cursor]), fake, letter_stack);
    free(letter_stack);
  }
  if (!fake)
    buffer[cursor] = '}';
  cursor++;
  return cursor;
}
