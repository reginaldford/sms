// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

/// A pointer that may also allow metadata to be associated with any object
/// @note The data field has not been used for anything yet.
/// @note When evaluated, the field is returned.
typedef struct sm_meta {
  int        my_type;
  sm_object *address;
  sm_cx     *data;
} sm_meta;

/// Construct a new meta
sm_meta *sm_new_meta(sm_object *address);
/// Convert this meta to a string (ignores data field for now)
sm_string *sm_meta_to_string(sm_meta *meta);
/// Print this meta to the string buffer
uint32_t sm_meta_sprint(sm_meta *self, char *buffer, bool fake);
