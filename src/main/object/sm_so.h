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

//
//
/** @brief  Shared object structure
 @note Null termination character excluded in size field.
 @note Zero size ff_fun still has NULL termination. */
typedef struct sm_ff_fun {
  uint32_t          my_type;
  struct sm_ff_fun *fun;
  struct sm_ff_sig *signature;
} sm_ff_fun;

/// Construct a new shared object
sm_ff_fun *sm_new_ff_fun(void *sm_handle);
/// Print to ff_fun buffer
uint32_t sm_ff_fun_sprint(sm_ff_fun *self, char *buffer, bool fake);
/// Returns whether two ff_funs match handle
bool sm_ff_fun_is_equal(sm_ff_fun *f1, sm_ff_fun *f2);
