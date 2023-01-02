// sm_table_entry structs follow the sm_context struct in memory
typedef struct sm_context {
  enum object_type   my_type;
  struct sm_context *parent;
  unsigned int       size;
  unsigned int       capacity;
} sm_context;

// key and value pair
typedef struct sm_table_entry {
  sm_string *name;
  sm_object *value;
} sm_table_entry;

// search_result allows binary search to return "would be here"
// by setting found to false and using index to indicate where it would be
typedef struct search_result {
  bool found;
  int  index;
} search_result;

sm_context            *sm_global_context(sm_context *replacement);
sm_context            *sm_new_context(unsigned int capacity);
struct sm_table_entry *get_table_entries(struct sm_context *context);
void                   sm_print_table(struct sm_context *context);
search_result          sm_find_var_index(struct sm_context *context, struct sm_string *var_string);
sm_context            *sm_set_var(struct sm_context *context, struct sm_string *name, void *val);
bool                   sm_delete(sm_symbol *sym);
sm_string             *sm_context_to_string(sm_context *self);
sm_string             *sm_table_entry_to_string(sm_table_entry *te);
