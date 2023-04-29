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

// Return a new cx node. Zero the child pointers first.
sm_cx_node *sm_new_cx_node(sm_object *value) {
  sm_cx_node *new_node = sm_malloc(sizeof(sm_cx_node));
  new_node->my_type    = SM_CX_NODE_TYPE;
  new_node->value      = value;
  new_node->map        = 0LL;
  new_node->children   = NULL;
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

// Inverse of sm_cx_node_index
// Expects 0-63, else, returns NULL
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

// Returns the node addressed by needle
// Returns NULL if this node does not exist
sm_cx_node *sm_cx_node_get_leaf(sm_cx_node *self, char *needle, int len) {
  sm_cx_node *curr_node = self;
  if (self && self->children == NULL)
    return NULL;
  // Traverse the trie to find the node corresponding to the key
  int index;
  int child_index;
  for (int i = 0; i < len; i++) {
    index       = sm_cx_node_index(needle[i]);
    if(curr_node==NULL) return NULL;
    child_index = sm_cx_node_child_index(curr_node->map, index);
    if (sm_cx_node_map_get(curr_node->map, index) == false)
      return NULL;
    struct sm_link *next_link = sm_link_nth(curr_node->children, child_index);
    if (next_link == NULL)
      return NULL;
    curr_node = (sm_cx_node *)next_link->value;
    if (curr_node == NULL)
      return NULL;
  }
  // Return the leaf node associated with the key
  return curr_node;
}

// Searches for an existing value and sets it to the given value and returns True.
// Returns false if the value cannot be found. Traverses parent ptrs to find the value.
bool sm_cx_set(sm_cx *self, char *needle, int len, sm_object *value) {
  sm_cx *cx = self;
  while (cx != NULL) {
    sm_cx_node *node = cx->content;
    sm_cx_node *leaf = sm_cx_node_get_leaf(node, needle, len);
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
  sm_cx_node *node = self->content;
  if (node != NULL) {
    sm_cx_node *leaf = sm_cx_node_get_leaf(node, needle, len);
    if (leaf != NULL)
      return leaf->value;
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
    if (curr_cx->content != NULL) {
      sm_cx_node *leaf = sm_cx_node_get_leaf(curr_cx->content, needle, len);
      if (leaf != NULL)
        return leaf->value;
    }
    curr_cx = curr_cx->parent;
  }
  return NULL;
}

// Search through this cx and ancestry for this key.
// Return the containing CX if the key is found, else return false.
sm_cx *sm_cx_get_container(sm_cx *self, char *needle, int len) {
  sm_cx *current_loc = self;
  while (current_loc) {
    sm_object *result = sm_cx_node_get(current_loc->content, needle, len);
    if (result)
      return current_loc;
    current_loc = current_loc->parent;
  }
  return NULL;
}

// Add a key_value with this key and value
bool sm_cx_let(sm_cx *self, char *needle, int len, sm_object *val) {
  sm_cx_node *cur_node;
  if (self->content == NULL)
    self->content = sm_new_cx_node(NULL);
  cur_node = self->content;
  for (int i = 0; i < len; i++) {
    int             index       = sm_cx_node_index(needle[i]);
    int             child_index = sm_cx_node_child_index(cur_node->map, index);
    struct sm_link *next_link;
    if (sm_cx_node_map_get(cur_node->map, index) == false) {
      sm_cx_node_map_set(&cur_node->map, index, true);
      next_link = sm_new_link((sm_object *)sm_new_cx_node(NULL), NULL);
      if (child_index == 0) {
        next_link->next    = cur_node->children;
        cur_node->children = next_link;
      } else
        sm_link_insert(cur_node->children, next_link, child_index);
    } else
      next_link = sm_link_nth(cur_node->children, child_index);
    cur_node = (sm_cx_node *)next_link->value;
  }
  // Leaf node already exists with this key
  if (cur_node->value != NULL)
    return false;
  cur_node->value = (sm_object *)val;
  return true;
}

// Report of the node has not value and no children
bool sm_cx_node_is_empty(sm_cx_node *node) { return node->value == NULL && node->map == 0LL; }

// Remove the node addressed by needle and return true.
// Return false if the node is not found.
bool sm_cx_rm(sm_cx *self, char *needle, int len) {
  sm_cx_node *cur_node = self->content;
  if (cur_node == NULL || cur_node->map == 0LL)
    return false;
  struct sm_link *cur_link  = cur_node->children;
  struct sm_link *last_link = cur_link;

  // Traverse the tree to find the node for the given key
  int index;
  int child_index;

  for (int i = 0; i < len; i++) {
    index       = sm_cx_node_index(needle[i]);
    child_index = sm_cx_node_child_index(cur_node->map, index);
    if (sm_cx_node_map_get(cur_node->map, index) == false) {
      // Node for this character does not exist
      return false;
    }
    last_link = cur_link;
    cur_link  = sm_link_nth(cur_node->children, child_index);
    cur_node  = (sm_cx_node *)cur_link->value;
  }
  // If the value for the final node is NULL, return false
  if (cur_node->value == NULL) {
    return false;
  }
  // Remove the value from the final node
  cur_node->value = NULL;
  // update the map
  if (last_link != cur_link)
    sm_cx_node_map_set(&((sm_cx_node *)last_link->value)->map, index, false);
  // If the final node has no children and no value, remove its link from the linked list
  if (sm_cx_node_is_empty(cur_node)) {
    if (last_link != cur_link) {
      last_link->next = last_link->next->next;
    } else {
    }
  }
  return true;
}

// Prints all of the key-value pairs in this node recursively
// Uses the stack to recall path to current node, for full key name
int sm_cx_node_sprint(sm_cx_node *node, char *buffer, bool fake, sm_stack *char_stack) {
  int cursor = 0;
  if (node->value != NULL) {
    // var name
    for (unsigned int i = sm_stack_size(char_stack) - 1; i + 1 > 0; i--) {
      sm_double *num_obj = *((sm_stack_empty_top(char_stack) + i + 1));
      if (!fake)
        buffer[i] = sm_cx_node_unindex(num_obj->value);
    }
    cursor = sm_stack_size(char_stack);
    // equals sign
    if (!fake)
      buffer[cursor] = '=';
    cursor++;
    // rhs value
    cursor += sm_object_sprint(node->value, &(buffer[cursor]), fake);
    // semicolon
    if (!fake)
      buffer[cursor] = ';';
    cursor++;
  }
  // If there are not more children, we are done
  if (sm_cx_node_is_empty(node)) {
    return cursor;
  }
  int items_to_do = sm_cx_node_map_size(node->map);
  for (int i = 0; items_to_do > 0 && i < 64; i++) {
    if (sm_cx_node_map_get(node->map, i) == true) {
      int         child_index = sm_cx_node_child_index(node->map, i);
      sm_cx_node *child_here  = (sm_cx_node *)sm_link_nth(node->children, child_index)->value;
      sm_stack_push(char_stack, sm_new_double(i));
      cursor += sm_cx_node_sprint(child_here, &(buffer[cursor]), fake, char_stack);
      sm_stack_pop(char_stack);
      items_to_do--;
    }
  }
  return cursor;
}

// Print the contents of this cx to buffer and return the length
// If fake is true, just return the hypotehtical string length
unsigned int sm_cx_sprint(sm_cx *self, char *buffer, bool fake) {
  if (!fake)
    buffer[0] = '{';
  int cursor = 1;
  if (self->content != NULL) {
    sm_stack *letter_stack = sm_new_stack(32);
    cursor += sm_cx_node_sprint(self->content, &(buffer[cursor]), fake, letter_stack);
    free(letter_stack);
  }
  if (!fake)
    buffer[cursor] = '}';
  cursor++;
  return cursor;
}

// Returns the number of children
int sm_cx_node_map_size(unsigned long long map) { return __builtin_popcountll(map); }

// Sets a bit of map to 1 or 0 depending on the provided boolean
void sm_cx_node_map_set(unsigned long long *map, int index, bool on) {
  if (on) {
    *map |= (1LL << index);
  } else {
    *map &= ~(1LL << index);
  }
}

// Return whether a bit is 1
bool sm_cx_node_map_get(unsigned long long map, int i) {
  unsigned long long mask = 1ULL << i;
  return (map & mask) != 0;
}

// Returns the correlating child index to this bit in the map
int sm_cx_node_child_index(unsigned long long map, int bit_index) {
  return __builtin_popcountll(map & ((1LL << bit_index) - 1));
}
