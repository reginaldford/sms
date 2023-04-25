// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"

// Create a new context, an array of key_values sorted by key
sm_cx *sm_new_cx(sm_cx *parent) {
  sm_cx *new_cx           = (sm_cx *)sm_malloc(sizeof(sm_cx));
  new_cx->my_type         = SM_CX_TYPE;
  new_cx->parent          = parent;
  new_cx->content.my_type = SM_CX_NODE_TYPE;
  new_cx->content.value   = NULL;
  for (int i = 0; i < 64; i++)
    new_cx->content.child[i] = NULL;
  return new_cx;
}

// Return a new cx node. Zero the child pointers first.
sm_cx_node *sm_new_cx_node(sm_object *value) {
  sm_cx_node *new_node = sm_malloc(sizeof(sm_cx_node));
  new_node->my_type    = SM_CX_NODE_TYPE;
  new_node->value      = value;
  for (int i = 0; i < 64; i++) {
    new_node->child[i] = NULL;
  }
  return new_node;
}

// Return the cx_node child index correlating to this character
// Char   ASCII   Group_Size
// '   is 39      1
// 0-9 is 48-57   10
// A-Z is 65-90   26
// _   is 95      1
// a-z is 98-122  26
// Total: 64
int sm_cx_node_index(char c) {
  int code = (int)c;
  if (code == 39)
    return 0;
  if (code >= 48 && code <= 57)
    return code - 48 + 1;
  if (code >= 65 && code <= 90)
    return code - 65 + 11;
  if (code == 95)
    return code - 95 + 37;
  if (code >= 97 && code <= 122)
    return code - 97 + 38;
  return -1;
}

// inverse of index
char sm_cx_node_unindex(int i) {
  if (i == 0)
    return '\'';
  if (i >= 1 && i <= 10)
    return i - 1 + '0';
  if (i >= 11 && i <= 36)
    return i - 11 + 'A';
  if (i == 37)
    return '_';
  if (i >= 38 && i <= 63)
    return i - 38 + 'a';
  return '\0';
}


sm_cx_node *sm_cx_node_get_leaf(sm_cx_node *self, char *needle, int len) {
  sm_cx_node *curr_node = self;
  int         index;
  // Traverse the trie to find the node corresponding to the key
  for (int i = 0; i < len; i++) {
    index     = sm_cx_node_index(needle[i]);
    curr_node = curr_node->child[index];
    if (curr_node == NULL)
      return NULL; // Key doesn't exist in the tree
  }
  // Return the leaf node associated with the key
  return curr_node;
}

//
bool sm_cx_set(sm_cx *self, char *needle, int len, sm_object *value) {
  sm_cx *cx = self;
  while (cx != NULL) {
    sm_cx_node *node = &(cx->content);
    sm_cx_node *leaf = sm_cx_node_get_leaf(node, needle, len);
    if (leaf != NULL) {
      leaf->value = value;
      return true;
    }
    cx = cx->parent;
  }
  return false;
}

//
sm_object *sm_cx_get(sm_cx *self, char *needle, int len) {
  sm_cx *cx = self;
  while (cx != NULL) {
    sm_cx_node *node = &(cx->content);
    sm_cx_node *leaf = sm_cx_node_get_leaf(node, needle, len);
    if (leaf != NULL)
      return leaf->value;
    cx = cx->parent;
  }
  return NULL;
}

// Get the object associated with this string
// Or return NULL.
sm_object *sm_cx_node_get(sm_cx_node *self, char *needle, int len) {
  sm_cx_node *leaf = sm_cx_node_get_leaf(self, needle, len);
  if (leaf)
    return leaf->value;
  else
    return NULL;
}

// Like sm_cx_get, but
// traverses the context ancestry to find the result, or returns NULL
sm_object *sm_cx_get_far(sm_cx *self, char *needle, int len) {
  sm_cx *curr_cx = self;
  while (curr_cx != NULL) {
    sm_cx_node *leaf = sm_cx_node_get_leaf(&curr_cx->content, needle, len);
    if (leaf != NULL)
      return leaf->value;
    curr_cx = curr_cx->parent;
  }
  return NULL;
}

// Search through this cx and ancestry for this key.
// Return the containing CX if the key is found, else return false.
sm_cx *sm_cx_get_container(sm_cx *self, char *needle, int len) {
  sm_cx *current_loc = self;
  while (current_loc) {
    sm_object *result = sm_cx_node_get(&current_loc->content, needle, len);
    if (result)
      return current_loc;
    current_loc = current_loc->parent;
  }
  return NULL;
}

// Add a key_value with this key and value
bool sm_cx_let(sm_cx *self, char *needle, int len, sm_object *val) {
  sm_cx_node *cur_node = &self->content;
  for (int i = 0; i < len; i++) {
    int index = sm_cx_node_index(needle[i]);
    if (cur_node->child[index] == NULL)
      cur_node->child[index] = sm_new_cx_node(NULL);
    cur_node = cur_node->child[index];
  }
  // Leaf node already exists with this key
  if (cur_node->value != NULL)
    return false;
  cur_node->value = (sm_object *)val;
  return true;
}

int sm_cx_node_last_index(sm_cx_node *node) {
  for (int i = 64; i >= 0; i--)
    if (node->child[i] == node)
      return i;
  return -1;
}

// Report whether all elements of the
bool sm_cx_node_is_empty(sm_cx_node *node) {
  for (int i = 0; i < 64; i++)
    if (node->child[i] != NULL)
      return false;
  return true;
}

// Remove the node addressed by needle and return true.
// Return false if the node is not found.
bool sm_cx_rm(sm_cx *self, char *needle, int len) {
  sm_cx_node *curr = &(self->content);
  for (int i = 0; i < len; i++) {
    if (curr == NULL)
      return false;
    curr = curr->child[sm_cx_node_index(needle[i])];
  }
  if (curr == NULL)
    return false;
  curr = NULL;
  return true;
}

int sm_cx_node_sprint(sm_cx_node *node, char *buffer, bool fake, sm_stack *char_stack) {
  int cursor = 0;
  if (node->value != NULL) {
    for (unsigned int i = sm_stack_size(char_stack) - 1; i + 1 > 0; i--) {
      sm_double *num_obj = *((sm_stack_empty_top(char_stack) + i + 1));
      if (!fake)
        buffer[i] = sm_cx_node_unindex(num_obj->value);
    }
    cursor = sm_stack_size(char_stack);

    if (!fake)
      buffer[cursor] = '=';
    cursor++;

    cursor += sm_object_sprint(node->value, &(buffer[cursor]), fake);
    if (!fake)
      buffer[cursor] = ';';
    cursor++;
  }
  if (sm_cx_node_is_empty(node)) {
    return cursor;
  }
  for (int i = 0; i < 64; i++) {
    if (node->child[i] != NULL) {
      sm_stack_push(char_stack, sm_new_double(i));
      cursor += sm_cx_node_sprint(node->child[i], &(buffer[cursor]), fake, char_stack);
      sm_stack_pop(char_stack);
    }
  }
  return cursor;
}

// For now
unsigned int sm_cx_sprint(sm_cx *self, char *buffer, bool fake) {
  sm_stack *new_stack = sm_new_stack(32);
  if (!fake)
    buffer[0] = '{';
  int cursor = 1;
  cursor += sm_cx_node_sprint(&(self->content), &(buffer[cursor]), fake, new_stack);
  if (!fake)
    buffer[cursor] = '}';
  cursor++;
  free(new_stack);
  return cursor;
}
