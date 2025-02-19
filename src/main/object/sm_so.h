// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

/** @brief  Shared object structure
 @note Null termination character excluded in size field.
 @note Zero size so still has NULL termination. */
typedef struct sm_so {
  uint32_t my_type;
  uint32_t type;
  void    *handle;
} sm_so;

/// Construct a new shared object
sm_so *sm_new_so(void *sm_handle);
/// Print to so buffer
uint32_t sm_so_sprint(sm_so *self, char *buffer, bool fake);
/// Returns whether two sos match handle
bool sm_so_is_equal(sm_so *str1, sm_so *str2);
