
// sms object to wrap the cif
typedef struct sm_ff_sig {
  uint32_t my_type;
  ffi_cif  cif;
} sm_ff_sig;


uint32_t          sm_ff_sig_sprint(sm_ff_sig *self, char *to_str, bool fake);
struct sm_ff_sig *sm_new_ff_sig(ffi_cif cif);
