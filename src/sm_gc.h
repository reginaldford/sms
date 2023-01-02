// garbage collected object
typedef struct sm_gco {
  int        my_type;
  sm_object *new_address;
} sm_gco;
void       sm_garbage_collect();
sm_object *sm_move_to_new_heap(sm_object *obj);
sm_object *sm_maybe_gc(sm_object *obj);
void       sm_inflate_heap();
