// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"

// Create a new sm_node with the given size
sm_node *sm_new_node(sm_object *value, struct sm_node *next, long long map,
                     struct sm_node *children) {
  sm_node *node  = sm_malloc(sizeof(sm_node));
  node->my_type  = SM_NODE_TYPE;
  node->value    = value;
  node->next     = next;
  node->map      = map;
  node->children = children;
  return node;
}

// Return the sm_node child index correlating to this character
// Char   ASCII   Group_Size
// '   is 39      1
// 0-9 is 48-57   10
// A-Z is 65-90   26
// _   is 95      1
// a-z is 98-122  26
// Total: 64
int sm_node_map_index(char c) {
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

// Inverse of sm_map_index
// Expects 0-63, else, returns NULL
char sm_node_bit_unindex(int i) {
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

// Return the child node, specified by index
sm_node *sm_node_nth(sm_node *self, int index) {
  sm_node *current = self;
  while (index > 0 && current != NULL) {
    current = current->next;
    index--;
  }
  if (index > 0)
    return NULL;
  return current;
}

// Identifies a node by its pointer, removes it and returns the new root
// Returns NULL if the node is not found
struct sm_node *sm_node_rm(struct sm_node *root, struct sm_node *nodeToRemove) {
  struct sm_node *prev    = NULL;
  struct sm_node *current = root;
  while (current != NULL && current != nodeToRemove) {
    prev    = current;
    current = current->next;
  }
  if (current == NULL) {
    // node not found in list
    return root;
  } else if (prev == NULL) {
    // node to remove is the first node in the list
    root = current->next;
  } else {
    // node to remove is not the first node in the list
    prev->next = current->next;
  }
  return root;
}

// Insert this node at position specified by where, else return false
bool sm_node_insert(struct sm_node *root, struct sm_node *new_node, int where) {
  struct sm_node *cur = root;
  int             i   = 0;
  while (cur && cur->next && i < where - 1) {
    cur = cur->next;
    i++;
  }
  if (i < where - 1)
    return false;
  new_node->next = cur->next;
  cur->next      = new_node;
  return true;
}

// Returns the number of set bits to the left of map_index'th bit in map
// Put in 64 for the number of 1 bits in the long long
int sm_node_map_left_count(unsigned long long map, int bit_index) {
  if (bit_index < 0) {
    return -1;
  }
  return __builtin_popcountll(map & ((1LL << bit_index) - 1));
}

// Return the node of the trie addressed by needle, or return NULL
sm_node* sm_node_subnode(sm_node* self, char* needle, int len) {
  sm_node* curr_node = self;
  int char_index = 0;
  while (char_index < len && curr_node != NULL) {
    int map_index = sm_node_map_index(needle[char_index]);
    unsigned long long map = curr_node->map;
    unsigned long long bit = 1ULL << map_index;
    if ((map & bit) == 0) {
      return NULL;
    }
    int child_index = sm_node_map_left_count(map, map_index);
    sm_node* child_here = (sm_node*)sm_node_nth(curr_node->children, child_index);
    curr_node = child_here;
    char_index++;
  }
  return (char_index == len) ? curr_node : NULL;
}

// Return the parent node of the node addressed by needle, or return NULL
sm_node *sm_node_parent_node(sm_node *self, char *needle, int len) {
  sm_node *curr_node   = self;
  sm_node *last_node   = NULL;
  int      map_index   = 0;
  int      child_index = 0;
  int      char_index  = 0;
  for (; char_index < len && curr_node != NULL; char_index++) {
    map_index = sm_node_map_index(needle[char_index]);
    if (sm_node_map_get(curr_node->map, map_index) == false)
      return NULL;
    child_index = sm_node_map_left_count(curr_node->map, map_index);
    last_node   = curr_node;
    curr_node   = sm_node_nth(curr_node->children, child_index);
    if (curr_node == NULL)
      return NULL;
  }
  if (char_index == len)
    return last_node;
  return NULL;
}

// Get this value from the current context.
// Return Null if there is no such key.
sm_object *sm_node_get(sm_node *self, char *needle, int len) {
  sm_node *node = self;
  if (node != NULL) {
    sm_node *leaf = sm_node_subnode(node, needle, len);
    if (leaf != NULL)
      return leaf->value;
  }
  return NULL;
}

// Get container of this value from the current context.
// Return Null if there is no such key.
sm_node *sm_node_get_container(sm_node *self, char *needle, int len) {
  sm_node *node = self;
  if (node != NULL) {
    sm_node *leaf = sm_node_subnode(node, needle, len);
    if (leaf != NULL)
      return leaf;
  }
  return NULL;
}

// Report of the node has not value and no children
bool sm_node_is_empty(sm_node *node) { return node->value == NULL && node->map == 0LL; }


// Prints all of the key-value pairs in this node recursively
// Uses the stack to recall path to current node, for full key name
int sm_node_sprint(sm_node *node, char *buffer, bool fake, sm_stack *char_stack) {
  int cursor = 0;
  if (node->value != NULL) {
    // var name
    for (unsigned int i = sm_stack_size(char_stack) - 1; i + 1 > 0; i--) {
      sm_double *num_obj = *((sm_stack_empty_top(char_stack) + i + 1));
      if (!fake)
        buffer[i] = sm_node_bit_unindex(num_obj->value);
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
  int items_to_do = sm_node_map_size(node->map);
  for (int i = 0; items_to_do > 0 && i < 8; i++) {
    char current_byte = ((char *)&(node->map))[i];
    if (current_byte != '\0')
      for (int j = 0; items_to_do > 0 && j < 8; j++) {
        int current_bit = 8 * i + j;
        if (sm_node_map_get(node->map, current_bit) == true) {
          int      child_index = sm_node_child_index(node->map, current_bit);
          sm_node *child_here  = (sm_node *)sm_node_nth(node->children, child_index);
          sm_stack_push(char_stack, sm_new_double(current_bit));
          cursor += sm_node_sprint(child_here, &(buffer[cursor]), fake, char_stack);
          sm_stack_pop(char_stack);
          items_to_do--;
        }
      }
  }
  return cursor;
}

// Return the number of children
int sm_node_map_size(unsigned long long map) { return __builtin_popcountll(map); }

// Set a bit of map to 1 or 0 depending on the provided boolean
void sm_node_map_set(unsigned long long *map, int index, bool on) {
  if (on) {
    *map |= (1LL << index);
  } else {
    *map &= ~(1LL << index);
  }
}

// Return whether a bit is 1
bool sm_node_map_get(unsigned long long map, int i) {
  unsigned long long mask = 1ULL << i;
  return (map & mask) != 0;
}

// Return the correlating child index to this bit in the map
int sm_node_child_index(unsigned long long map, int map_index) {
  return __builtin_popcountll(map & ((1LL << map_index) - 1));
}

bool sm_node_rm_nth(struct sm_node *root, int n) {
  struct sm_node *cur = root;
  int             i   = 0;
  while (cur->next && i < n - 1) {
    cur = cur->next;
    i++;
  }
  if (i < n - 1 || cur->next == NULL) {
    return false; // Not enough nodes in the list
  }
  cur->next = cur->next->next;
  return true;
}

// Returns the number of values under this node (recursive)
int sm_node_size(sm_node *node) {
  int size = 0;
  if (node->value != NULL)
    size++;
  unsigned long long map = node->map; // Get the bitmap
  while (map != 0) {
    unsigned long long bit = map & -map;    // Using two's complement trick
    int bit_index   = __builtin_ctzll(bit); // Using built-in ctzll (count trailing zeros) function
    int child_index = sm_node_child_index(node->map, bit_index);
    sm_node *child_here = (sm_node *)sm_node_nth(node->children, child_index);
    size += sm_node_size(child_here);
    map ^= bit; // Clear the bit
  }

  return size;
}


// Returns the keys under this node(recursive)
sm_expr *sm_node_keys(sm_node *node, sm_stack *char_stack, sm_expr *collection) {
  if (node == NULL)
    return sm_new_expr_n(SM_ARRAY_EXPR, 0, 0);

  if (node->value != NULL) {
    // Build the key string
    int  len = sm_stack_size(char_stack);
    char buffer[len + 1]; // Increase the buffer size to accommodate the null terminator
    buffer[len] = '\0';   // Add the null terminator at the end

    for (unsigned int i = sm_stack_size(char_stack) - 1; i + 1 > 0; i--) {
      sm_double *num_obj = *((sm_stack_empty_top(char_stack) + i + 1));
      buffer[i]          = sm_node_bit_unindex(num_obj->value);
    }

    // Add the key to the collection
    collection = sm_expr_append(collection, (sm_object *)sm_new_symbol(sm_new_string(len, buffer)));
  }

  // If there are no more children, we are done
  if (sm_node_is_empty(node)) {
    return collection;
  }

  unsigned long long map = node->map; // Get the bitmap

  while (map != 0) {
    unsigned long long bit = map & -map; // Get the rightmost set bit using two's complement trick
    int                bit_index = __builtin_ctzll(
      bit); // Get the index of the set bit using built-in ctzll (count trailing zeros) function

    int      child_index = sm_node_child_index(node->map, bit_index);
    sm_node *child_here  = (sm_node *)sm_node_nth(node->children, child_index);

    sm_stack_push(char_stack, sm_new_double(bit_index));
    collection = sm_node_keys(child_here, char_stack, collection);
    sm_stack_pop(char_stack);

    map ^= bit; // Clear the current bit
  }

  return collection;
}


// Returns the keys under this node(recursive)
sm_expr *sm_node_values(sm_node *node, sm_expr *collection) {
  if (node == NULL)
    return sm_new_expr_n(SM_ARRAY_EXPR, 0, 0);
  if (node->value != NULL) {
    // var name
    collection = sm_expr_append(collection, node->value);
  }
  // If there are not more children, we are done
  if (sm_node_is_empty(node)) {
    return collection;
  }
  int items_to_do = sm_node_map_size(node->map);
  for (int i = 0; items_to_do > 0 && i < 64; i++) {
    if (sm_node_map_get(node->map, i) == true) {
      int      child_index = sm_node_child_index(node->map, i);
      sm_node *child_here  = (sm_node *)sm_node_nth(node->children, child_index);
      collection           = sm_node_values(child_here, collection);
      items_to_do--;
    }
  }
  return collection;
}
