// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

// sm_context_entry structs follow the sm_context struct in memory
typedef struct sm_context {
  short int          my_type;
  struct sm_context *parent;
  unsigned int       size;
  unsigned int       capacity;
  sm_expr           *children;
} sm_context;

// key and value pair
typedef struct sm_context_entry {
  sm_string *name;
  sm_object *value;
} sm_context_entry;

// sm_search_result allows binary search to return "would be here"
// by setting found to false and using index to indicate where it would be
typedef struct sm_search_result {
  bool         found;
  unsigned int index;
} sm_search_result;

// For cascading searches.
// If the variable is found, the sm_contex containing the variable is set as 'context'
typedef struct sm_search_result_cascading {
  bool         found;
  unsigned int index;
  sm_context  *context;
} sm_search_result_cascading;

sm_context       *sm_new_context(unsigned int size, unsigned int capacity, sm_context *parent);
sm_context_entry *sm_context_entries(struct sm_context *context);
sm_search_result  sm_context_find_index(struct sm_context *context, struct sm_string *var_string);
sm_search_result_cascading sm_context_find_far(sm_context *context, sm_string *var_string);
sm_context  *sm_context_set(struct sm_context *context, struct sm_string *name, void *val);
bool         sm_context_rm(sm_context *self, sm_symbol *sym);
unsigned int sm_context_sprint(sm_context *self, char *buffer);
unsigned int sm_context_to_string_len(sm_context *self);
unsigned int sm_context_entry_sprint(sm_context_entry *ce, char *buffer);
unsigned int sm_context_entry_to_string_len(sm_context_entry *ce);
sm_context  *sm_context_add_child(sm_context *cx, sm_object *child);
bool         sm_context_update_relatives(sm_context *self, sm_context *old_self);
sm_object   *sm_context_get(sm_context *self, unsigned int index);
sm_object   *sm_context_get_by_name(sm_context *self, sm_string *name);
sm_object   *sm_context_get_by_name_far(sm_context *self, sm_string *name);