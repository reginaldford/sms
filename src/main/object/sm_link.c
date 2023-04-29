#include "../sms.h"

// Create a new sm_link, a part of a linked list
struct sm_link *sm_new_link(sm_object *value, struct sm_link *next) {
  struct sm_link *new_link = sm_malloc(sizeof(struct sm_link));
  new_link->my_type        = SM_LINK_TYPE;
  new_link->value          = value;
  new_link->next           = next;
  return new_link;
}

// Returns the first node with a value for which f(value)==true
struct sm_link *sm_link_find(struct sm_link *root, bool (*f)(struct sm_object *)) {
  struct sm_link *current = root;
  while (current != NULL) {
    if (f(current->value))
      return current;
    current = current->next;
  }
  return NULL;
}

// Return the nth node in the list
struct sm_link *sm_link_nth(struct sm_link *root, int n) {
  struct sm_link *current = root;
  int             i;
  for (i = 0; current != NULL && i < n; i++)
    current = current->next;
  if (i == n && current != NULL) {
    return current;
  } else
    return NULL;
}

// Identifies a node by its pointer, removes it and returns true
// Returns false if the node is not found
struct sm_link *sm_link_rm(struct sm_link *root, struct sm_link *nodeToRemove) {
  struct sm_link *prev    = NULL;
  struct sm_link *current = root;
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

// Insert this link at position specified by where, else return false
bool sm_link_insert(struct sm_link *root, struct sm_link *new_node, int where) {
  struct sm_link *cur = root;
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


bool sm_link_rm_nth(struct sm_link *root, int n) {
  struct sm_link *cur = root;
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
