// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

// linked list node
struct sm_link {
  int16_t           my_type;
  struct sm_object *value;
  struct sm_link   *next;
};

struct sm_link *sm_new_link(sm_object *value, struct sm_link *next);
struct sm_link *sm_link_find(struct sm_link *root, bool (*f)(struct sm_object *));
struct sm_link *sm_link_nth(struct sm_link *root, int n);
bool            sm_link_insert(struct sm_link *root, struct sm_link *new_node, int where);
bool            sm_link_rm_nth(struct sm_link *root, int n);
struct sm_link *sm_link_rm(struct sm_link *root, struct sm_link *nodeToRemove);
