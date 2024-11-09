// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"

extern sm_heap *sms_symbol_heap;

// Create a new sm_node
sm_node *sm_new_node(sm_object *value, struct sm_node *next, long long map,
                     struct sm_node *children, uint32_t sym_id) {
  sm_node *node   = sm_malloc_plain(sizeof(sm_node));
  node->my_type   = SM_NODE_TYPE;
  node->value     = value;
  node->next      = next;
  node->map       = map;
  node->children  = children;
  node->symbol_id = sym_id;
  return node;
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

// Insert this node at position specified by int where, else return false
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

// Define popcountll whether or not we have the built-in instruction.
#ifdef __x86_64__
int popcountll(uint64_t num) { return __builtin_popcountll(num); }
#else
// Manually counting the bits in the 64 bit int, efficiently
int popcountll(uint64_t num) {
  int count = 0;
  for (count = 0; num; count++)
    num &= (num - 1);
  return count;
}
#endif

// Return the number of set bits to the left of map_index'th bit in map
int sm_node_map_left_count(uint64_t map, int bit_index) {
  return popcountll(map & ((1LL << bit_index) - 1));
}

// Return the node of the trie addressed by needle, or return NULL
sm_node *sm_node_subnode(sm_node *self, char *needle, int len) {
  sm_node *curr_node  = self;
  int      char_index = 0;
  while (char_index < len && curr_node != NULL) {
    int      map_index = needle[char_index];
    uint64_t map       = curr_node->map;
    uint64_t bit       = 1ULL << map_index;
    if ((map & bit) == 0) {
      return NULL;
    }
    int child_index = sm_node_map_left_count(map, map_index);
    curr_node       = (sm_node *)sm_node_nth(curr_node->children, child_index);
    char_index++;
  }
  if (curr_node && !curr_node->value)
    return false;
  return curr_node;
}

// Return the parent node of the node addressed by needle, or return NULL
sm_node *sm_node_parent_node(sm_node *self, char *needle, int len) {
  sm_node *curr_node   = self;
  sm_node *last_node   = NULL;
  int      map_index   = 0;
  int      child_index = 0;
  int      char_index  = 0;
  for (; char_index < len && curr_node != NULL; char_index++) {
    map_index = needle[char_index];
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

// Print all of the key-value pairs in this node recursively
// Uses the stack to recall path to current node, for full key name
int sm_node_sprint(sm_node *node, char *buffer, bool fake, sm_stack_obj *char_stack) {
  int cursor = 0;
  if (node->value != NULL) {
    // first, write the symbol name
    uint32_t   sym_id = node->symbol_id;
    sm_symbol *sym    = &(((sm_symbol *)sms_symbol_heap->storage)[sym_id]);
    if (!fake)
      sm_strncpy(buffer, &sym->name->content, sym->name->size);
    cursor = sym->name->size;
    // arrow sign
    if (!fake) {
      buffer[cursor]     = '-';
      buffer[cursor + 1] = '>';
    }
    cursor += 2;
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
          sm_stack_obj_push(char_stack, sm_new_f64(current_bit));
          cursor += sm_node_sprint(child_here, &(buffer[cursor]), fake, char_stack);
          sm_stack_obj_pop(char_stack);
          items_to_do--;
        }
      }
  }
  return cursor;
}

// Return the number of children
int sm_node_map_size(uint64_t map) { return popcountll(map); }

// Set a bit of map to 1 or 0 depending on the provided boolean
void sm_node_map_set(uint64_t *map, int index, bool on) {
  if (on) {
    *map |= (1LL << index);
  } else {
    *map &= ~(1LL << index);
  }
}

// Return whether a bit is 1
bool sm_node_map_get(uint64_t map, int i) {
  uint64_t mask = 1ULL << i;
  return (map & mask) != 0;
}

// Return the correlating child index to this bit in the map
int sm_node_child_index(uint64_t map, int map_index) {
  return popcountll(map & ((1LL << map_index) - 1));
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
  uint64_t map = node->map; // Get the bitmap
  while (map != 0) {
    uint64_t bit    = map & -map;           // Using two's compliment trick
    int bit_index   = __builtin_ctzll(bit); // Using built-in ctzll (count trailing zeros) function
    int child_index = sm_node_child_index(node->map, bit_index);
    sm_node *child_here = (sm_node *)sm_node_nth(node->children, child_index);
    size += sm_node_size(child_here);
    map ^= bit; // Clear the bit
  }

  return size;
}

// Returns the keys under this node(recursive)
sm_expr *sm_node_keys(sm_node *node, sm_stack_obj *char_stack, sm_expr *collection) {
  if (node == NULL)
    return sm_new_expr_n(SM_TUPLE_EXPR, 0, 0, NULL);

  if (node->value != NULL) {
    sm_symbol *found_sym = &((sm_symbol *)sms_symbol_heap->storage)[node->symbol_id];
    // Add the key to the collection
    collection = sm_expr_append(collection, (sm_object *)found_sym);
  }

  // If there are no more children, we are done
  if (sm_node_is_empty(node)) {
    return collection;
  }

  uint64_t map = node->map; // Get the bitmap

  while (map != 0) {
    uint64_t bit       = map & -map; // Get the rightmost set bit using two's compliment
    int      bit_index = __builtin_ctzll(
           bit); // Get the index of the set bit using built-in ctzll (count trailing zeros) function

    int      child_index = sm_node_child_index(node->map, bit_index);
    sm_node *child_here  = (sm_node *)sm_node_nth(node->children, child_index);

    sm_stack_obj_push(char_stack, sm_new_f64(bit_index));
    collection = sm_node_keys(child_here, char_stack, collection);
    sm_stack_obj_pop(char_stack);
    map ^= bit; // Clear the current bit
  }

  return collection;
}

// Returns the keys under this node(recursive)
sm_expr *sm_node_values(sm_node *node, sm_expr *collection) {
  if (node == NULL)
    return sm_new_expr_n(SM_TUPLE_EXPR, 0, 0, NULL);
  if (node->value != NULL)
    collection = sm_expr_append(collection, node->value);
  // If there are not more children, we are done
  if (sm_node_is_empty(node))
    return collection;
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
