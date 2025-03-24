#include <ffi.h>
#include <stdio.h>
#include <dlfcn.h>
#include "sms.h"

// Create a new ffi signature object
struct sm_ff_sig *sm_new_ff_sig(ffi_cif cif) {
  struct sm_ff_sig *new_ff_sig =
    sm_malloc(sizeof(struct sm_ff_sig) + sizeof(ffi_type *) * cif.nargs);
  new_ff_sig->my_type = SM_FF_SIG_TYPE;
  new_ff_sig->cif     = cif;
  memcpy(new_ff_sig + 1, cif.arg_types, cif.nargs * sizeof(ffi_type *));
  return new_ff_sig;
}

struct sm_ff *sm_new_ff(void *fptr, sm_string *fname, sm_ff_sig *sig) {
  sm_ff *ff   = sm_malloc(sizeof(struct sm_ff) + sizeof(ffi_type *) * sig->cif.nargs);
  ff->my_type = SM_FF_TYPE;
  ff->fptr    = fptr;
  ff->name    = fname;
  ff->cif     = sig->cif;
  // Copy the arg_types pointers after the struct
  memcpy(ff + 1, sig + 1, sig->cif.nargs * sizeof(ffi_type *));
  // Update the arg_types ptr on the cif
  ff->cif.arg_types = (ffi_type **)(ff + 1);
  return ff;
}

// Array of FFI type names
ffi_type *get_ffi_type(const char *type) {
  const char *valid_type_strings[] = {
    "void",   "pointer", "uint8",  "uint16", "uint32", "uint64",        "sint8",
    "sint16", "sint32",  "sint64", "float",  "double", "complex_float", "complex_double",
  };

  size_t num_types = sizeof(valid_type_strings) / sizeof(valid_type_strings[0]);

  // Array of FFI type pointers, associated with the names defined above
  ffi_type *valid_types[] = {
    &ffi_type_void,          &ffi_type_pointer,        &ffi_type_uint8, &ffi_type_uint16,
    &ffi_type_uint32,        &ffi_type_uint64,         &ffi_type_sint8, &ffi_type_sint16,
    &ffi_type_sint32,        &ffi_type_sint64,         &ffi_type_float, &ffi_type_double,
    &ffi_type_complex_float, &ffi_type_complex_double,
  };
  // We search through teh string list, return the correlating ffi_type
  for (size_t i = 0; i < num_types; ++i) {
    if (strcmp(type, valid_type_strings[i]) == 0) {
      return valid_types[i];
    }
  }
  return NULL; // Type not found
}

ffi_type *sm_ffi_type_from_symbol(sm_symbol *sym) { return get_ffi_type(&(sym->name->content)); }

// If fake is false,
// Prints to to_str a string describing the ff_sig
// Returns the length regardless
uint32_t sm_ff_sig_sprint(sm_ff_sig *self, char *to_str, bool fake) {
  if (!fake)
    return sprintf(to_str, "(ff_sig@%p)", &self->cif);
  else {
    char tempBuffer[32];
    return snprintf(tempBuffer, 32, "(ff_sig@%p)", &self->cif);
  }
}

// If fake is false,
// Prints to to_str a string describing the ff
// Returns the length regardless
uint32_t sm_ff_sprint(sm_ff *self, char *to_str, bool fake) {
  if (!fake)
    return sprintf(to_str, "(ff@%p)", self->fptr);
  else {
    char tempBuffer[32];
    return snprintf(tempBuffer, 32, "(ff@%p)", self->fptr);
  }
}

// Returns whether two ff match handle ptr
bool sm_ff_is_equal(sm_ff *ff1, sm_ff *ff2) { return ff1->fptr == ff2->fptr; }

// Returns whether two ff_sigs match handle ptr
// bool sm_ff_sig_is_equal(sm_ff_sig *ff_sig1, sm_ff_sig *ff_sig2) { return
// ((void*)ff_sig1->cif)((void*)ff_sig2->cif); }
