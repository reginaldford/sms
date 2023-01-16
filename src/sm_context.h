// The following file is provided under the BSD 2-clause license. For more info, read LICENSE.txt.

// sm_context_entry structs follow the sm_context struct in memory
typedef struct sm_context {
  short int          my_type;
  struct sm_context *parent;
  unsigned int       size;
  unsigned int       capacity;
} sm_context;

// key and value pair
typedef struct sm_context_entry {
  sm_string *name;
  sm_object *value;
} sm_context_entry;

// search_result allows binary search to return "would be here"
// by setting found to false and using index to indicate where it would be
typedef struct search_result {
  bool         found;
  unsigned int index;
} search_result;

sm_context              *sm_global_context(sm_context *replacement);
sm_context              *sm_new_context(unsigned int capacity);
struct sm_context_entry *sm_context_entries(struct sm_context *context);
void                     sm_print_table(struct sm_context *context);
search_result sm_find_var_index(struct sm_context *context, struct sm_string *var_string);
sm_context   *sm_set_var(struct sm_context *context, struct sm_string *name, void *val);
bool          sm_delete(sm_symbol *sym);
sm_string    *sm_context_to_string(sm_context *self);
unsigned int  sm_context_sprint(sm_context *self, char *buffer);
unsigned int  sm_context_to_string_len(sm_context *self);
sm_string    *sm_context_entry_to_string(sm_context_entry *ce);
unsigned int  sm_context_entry_sprint(sm_context_entry *ce, char *buffer);
unsigned int  sm_context_entry_to_string_len(sm_context_entry *ce);
