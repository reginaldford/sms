// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"

extern sm_heap *sms_symbol_heap;

// Create a new ConteXt
sm_cx *sm_new_cx(sm_cx *parent) {
  sm_cx *new_cx   = (sm_cx *)sm_malloc(sizeof(sm_cx));
  new_cx->my_type = SM_CX_TYPE;
  new_cx->parent  = parent;
  new_cx->content = NULL;
  return new_cx;
}

// Search through this cx and ancestry for this key.
// Return the containing CX if the key is found, else return false.
sm_cx *sm_cx_get_container(sm_cx *self, sm_symbol *sym) {
  char  *needle      = &sym->code_id->content;
  int    len         = sym->code_id->size;
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
bool sm_cx_set(sm_cx *self, sm_symbol *sym, sm_object *value) {
  sm_cx *cx = self;
  while (cx != NULL) {
    sm_node *node = cx->content;
    sm_node *leaf = sm_node_subnode(node, &sym->code_id->content, sym->code_id->size);
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
sm_object *sm_cx_get(sm_cx *self, sm_symbol *sym) {
  sm_node *node = self->content;
  if (node != NULL) {
    char    *needle = &sym->code_id->content;
    int      len    = sym->code_id->size;
    sm_node *leaf   = sm_node_subnode(node, needle, len);
    if (leaf != NULL)
      return leaf->value;
  }
  return NULL;
}

// Like sm_cx_get, but
// traverses the context ancestry to find the result, or returns NULL
sm_object *sm_cx_get_far(sm_cx *self, sm_symbol *sym) {
  char  *needle  = &sym->code_id->content;
  int    len     = sym->code_id->size;
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
// sym_name is not managed by gc, so should be from sms_symbol_name_heap
bool sm_cx_let(sm_cx *self, sm_symbol *sym, sm_object *val) {
  sm_node *current_node;
  if (self->content == NULL)
    self->content = sm_new_node(NULL, NULL, 0LL, NULL, 0);
  current_node = self->content;
  for (int i = 0; i < sym->code_id->size; i++) {
    int             index       = (&sym->code_id->content)[i];
    int             child_index = sm_node_child_index(current_node->map, index);
    struct sm_node *next_node;
    if (sm_node_map_get(current_node->map, index) == false) {
      sm_node_map_set(&current_node->map, index, true);
      next_node = sm_new_node(NULL, NULL, 0LL, NULL, 0);
      if (child_index == 0) {
        next_node->next        = current_node->children;
        current_node->children = next_node;
      } else
        sm_node_insert(current_node->children, next_node, child_index);
    } else
      next_node = sm_node_nth(current_node->children, child_index);
    current_node = (sm_node *)next_node;
  }
  current_node->value     = (sm_object *)val;
  current_node->symbol_id = sym - (sm_symbol *)sms_symbol_heap->storage;
  return val;
}

// We return the last node in the path to the node addressed by needle
// which has a map size more than 1.
// If there is no map size more than 1 in the path, then the root node is returned.
bool sm_cx_rm(sm_cx *self, sm_symbol *sym) {
  char *needle = &sym->code_id->content;
  int   len    = sym->code_id->size;
  // First, determine that the node exists
  // While tracking the node_path for later
  sm_node *node_path[len + 1]; // we include the root node which has no letter associated
  sm_node *curr_node   = self->content;
  int      map_index   = 0;
  int      child_index = 0;
  int      char_index  = 0;
  node_path[0]         = curr_node;
  for (; char_index < len && curr_node != NULL; char_index++) {
    map_index = needle[char_index];
    if (sm_node_map_get(curr_node->map, map_index) == false)
      return false;
    child_index               = sm_node_map_left_count(curr_node->map, map_index);
    curr_node                 = sm_node_nth(curr_node->children, child_index);
    node_path[char_index + 1] = curr_node;
    if (curr_node == NULL)
      return false;
  }
  if (char_index < len)
    return false;
  // Delete the value at the addressed node
  curr_node->value = NULL;
  // The rest is garbage collection
  // For nodes addressed by subsets of needle from beginning to all other letters
  for (int i = len; i > 0; i--) {
    curr_node                        = node_path[i];
    struct sm_node *curr_node_parent = node_path[i - 1];
    if (curr_node->map == 0LL && curr_node->value == NULL) {
      map_index   = needle[i - 1];
      child_index = sm_node_map_left_count(curr_node_parent->map, map_index);
      sm_node_map_set(&curr_node_parent->map, map_index, false);
      curr_node_parent->children = sm_node_rm(curr_node_parent->children, curr_node);
    } else
      break;
  }
  // If the root node's map is empty, nullify this cx's content
  if (self->content->map == 0LL) {
    self->content = NULL;
  }
  return true;
}

// Print the contents of this cx to buffer and return the length
// If fake is true, just return the hypothetical string length
uint32_t sm_cx_sprint(sm_cx *self, char *buffer, bool fake) {
  if (!fake)
    buffer[0] = '{';
  int cursor = 1;
  if (self->content != NULL) {
    sm_stack_obj *letter_stack = sm_new_stack_obj(7);
    int           len          = sm_node_sprint(self->content, &buffer[cursor], fake, letter_stack);
    cursor += len;
  }
  if (!fake)
    buffer[cursor] = '}';
  cursor++;
  return cursor;
}

// Clear the contents of the cx
void sm_cx_clear(sm_cx *self) { self->content = NULL; }

// Number of elements in the cx
uint32_t sm_cx_size(sm_cx *self) {
  if (self->content != NULL)
    return sm_node_size(self->content);
  return 0;
}

// Uses cxLet to import key/value pairs from cxFrom to cxTo
void sm_cx_import(sm_cx *cxFrom, sm_cx *cxTo) {
  sm_expr *keys =
    sm_node_keys(cxFrom->content, sm_new_stack_obj(32), sm_new_expr_n(SM_TUPLE_EXPR, 0, 0, NULL));
  sm_expr *values = sm_node_values(cxFrom->content, sm_new_expr_n(SM_TUPLE_EXPR, 0, 0, NULL));
  for (uint32_t i = 0; i < keys->size; i++) {
    sm_symbol *keySym = (sm_symbol *)sm_expr_get_arg(keys, i);
    sm_cx_let(cxTo, keySym, sm_expr_get_arg(values, i));
  }
}
