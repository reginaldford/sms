#include <ffi.h>
#include <stdio.h>
#include <dlfcn.h>
#include "sms.h"

// Create a new ffi signature object
struct sm_ff_sig *sm_new_ff_sig(ffi_cif cif, uint32_t num_args) {
  struct sm_ff_sig *new_ff_sig =
    sm_malloc(sizeof(struct sm_ff_sig) + sizeof(ffi_type *) * num_args);
  new_ff_sig->my_type = SM_FF_SIG_TYPE;
  new_ff_sig->cif     = cif;
  return new_ff_sig;
}

struct sm_ff *sm_new_ff(sm_string *fname, sm_ff_sig *sig) {
  sm_ff *ff    = sm_malloc(sizeof(ff) + sizeof(void *) * sig->num_args);
  ff->my_type  = SM_FF_TYPE;
  ff->num_args = sig->num_args;
  ff->cif      = sig->cif;
  ff->name     = fname;
  memcpy((void *)((&ff) + 1), (void *)((&sig) + 1), sig->num_args * sizeof(void *));
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

// Create a new shard object
sm_ff_fun *sm_new_ff_fun(void *handle) {
  struct sm_ff_fun *new_ff_fun = sm_malloc(sizeof(sm_ff_fun));
  new_ff_fun->my_type          = SM_FF_TYPE;
  new_ff_fun->fun              = handle;
  return new_ff_fun;
}

// If fake is false,
// Prints to to_str a string describing the ff_fun
// Returns the length regardless
uint32_t sm_ff_fun_sprint(sm_ff_fun *self, char *to_str, bool fake) {
  if (!fake)
    return sprintf(to_str, "(ff_fun@%p)", self->fun);
  else {
    char tempBuffer[32];
    return snprintf(tempBuffer, 32, "(ff_fun@%p)", self->fun);
  }
}

// Returns whether two ff_funs match handle ptr
bool sm_ff_fun_is_equal(sm_ff_fun *ff_fun1, sm_ff_fun *ff_fun2) {
  return ff_fun1->fun == ff_fun2->fun;
}

// Returns whether two ff_sigs match handle ptr
// bool sm_ff_sig_is_equal(sm_ff_sig *ff_sig1, sm_ff_sig *ff_sig2) { return
// ((void*)ff_sig1->cif)((void*)ff_sig2->cif); }
