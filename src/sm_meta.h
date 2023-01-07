typedef struct sm_meta {
  int        my_type;
  int        meta_level;
  sm_object *address;
} sm_meta;

sm_meta *sm_new_meta(unsigned int meta_level, sm_object *address);
