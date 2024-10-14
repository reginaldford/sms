// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

/// Copy the object
sm_object *sm_copy(sm_object *obj);
/// Deep Copy the object
sm_object *sm_deep_copy(sm_object *obj);
/// Copies all objects linked to _scratch to new space. Uses "stop and copy" method.
void sm_garbage_collect();
/// Copy the object to the new heap, mutating the original object to an sm_pointer.
sm_object *sm_move_to_new_heap(sm_heap *dest, sm_object *obj);
/// Completes the sm_garbage_collect() process by 'inflating' the existing objects in the heap.
void sm_inflate_heap(sm_heap *from, sm_heap *to);
/// If obj is an sm_pointer, the object was already moved to the new heap
/// Else, copy the object to the new heap and leave an sm_pointer
sm_object *sm_meet_object(sm_heap *source, sm_heap *dest, sm_object *obj);
