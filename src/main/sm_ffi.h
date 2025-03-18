// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

// sms object to wrap the cif
// There is an understood array of pointers to ffi_type objects following this struct
// The number of pointers should match num_args
typedef struct sm_ff_sig {
  uint32_t my_type;
  size_t   num_args;
  ffi_cif  cif;
} sm_ff_sig;

// Object with all info to quickly call a foreign function
// There is an understood array of pointers to ffi_type objects following this struct
// The number of pointers should match num_args
typedef struct sm_ff {
  uint32_t   my_type;
  void      *fptr;
  sm_string *name;
  size_t     num_args;
  ffi_cif    cif;
} sm_ff;

ffi_type         *sm_ffi_type_from_symbol(sm_symbol *sym);
uint32_t          sm_ff_sig_sprint(sm_ff_sig *self, char *to_str, bool fake);
struct sm_ff_sig *sm_new_ff_sig(ffi_cif cif, uint32_t num_args);
struct sm_ff     *sm_new_ff(void *fptr, sm_string *fname, sm_ff_sig *sig);
