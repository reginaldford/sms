// The following file is provided under the BSD 2-clause license. For more info, read LICENSE.txt.

typedef struct sm_meta {
  int         my_type;
  sm_object  *address;
  sm_context *data;
} sm_meta;

sm_meta     *sm_new_meta(sm_object *address);
sm_string   *sm_meta_to_string(sm_meta *meta);
unsigned int sm_meta_to_string_len(sm_meta *meta);
unsigned int sm_meta_sprint(sm_meta *self, char *buffer);
