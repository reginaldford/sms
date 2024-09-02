// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information
/// Contexts are data trees that map symbol names to values

typedef struct sm_cx {
  int32_t       my_type;
  struct sm_cx *parent;
  sm_node      *content;
} sm_cx;

/// Construct a new, empty context, with the given parent
///  @param parent The parent context (may be NULL)
sm_cx *sm_new_cx(sm_cx *parent);
/// Get the value associated with needle from self
/// @param self the context used in the lookup
/// @param needle The key value to find
/// @param len the string length of needle
sm_object *sm_cx_get(sm_cx *self, sm_symbol *sym);
/// If the value is not found in self, search in parent(self), and so on
sm_object *sm_cx_get_far(sm_cx *self, sm_symbol *sym);
/// Same as get_far, but returns the context which contains the key
sm_cx *sm_cx_get_container(sm_cx *self, sm_symbol *sym);
/// Create a new key-value association in context
bool sm_cx_let(sm_cx *context, sm_symbol *sym, sm_object *val);
/// Set an existing value in context
bool sm_cx_set(sm_cx *context, sm_symbol *sym, sm_object *val);
/// Remove a key-value pair identified by needle
bool sm_cx_rm(sm_cx *self, sm_symbol *sym);
/// If fake is false, print self into buffer.
/// @return The length of text printed.
uint32_t sm_cx_sprint(sm_cx *self, char *buffer, bool fake);
/// Construct a new context node, to store value (which may be NULL)
sm_node *sm_new_cx_node(sm_object *value);
/// Get the number of key-value pairs in this context
uint32_t sm_cx_size(sm_cx *self);
/// Remove all key-value pairs from this context
void sm_cx_clear(sm_cx *self);
/// Import all key-value pairs from cxFrom to cxTo
void sm_cx_import(sm_cx *cxFrom, sm_cx *cxTo);
/// Set the parent context to cx, if the object has a 'parent cx' ptr
void sm_cx_contextualize(sm_object *input, sm_cx *cx);
