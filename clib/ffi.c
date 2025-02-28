#include <dlfcn.h>
#include <string.h>
#include <stdio.h>
#include <ffi.h>
#include <memory.h>
#include "../src/main/sms.h"

// Helper function to map type strings to ffi_type pointers
static ffi_type *get_ffi_type(const char *type_str) {
  if (strcmp(type_str, "f64") == 0) {
    return &ffi_type_double;
  } else if (strcmp(type_str, "str") == 0) {
    return &ffi_type_pointer;
  }
  // Add more cases for other types as needed
  printf("Unsupported type: %s\n", type_str);
  return NULL;
}

sm_object *sm_ffi_call(sm_object *input, const char *function_name, const char *param_types) {
  void *lib_handle = dlopen("path/to/lib.so", RTLD_LAZY); // Replace with actual path
  if (!lib_handle) {
    printf("Failed to load library\n");
    return input;
  }

  void *func_ptr = dlsym(lib_handle, function_name);
  if (!func_ptr) {
    printf("Function %s not found\n", function_name);
    dlclose(lib_handle);
    return input;
  }

  int num_args = 5; // example


  ffi_arg   args[num_args];
  ffi_type type = ffi_type_double; //example
  ffi_type result_type = ffi_type_double; //example
  ffi_cif cif;
  void   *result;

  if (ffi_prep_cif(&cif, func_ptr, &result, num_args, type_strings) != FFI_OK) {
    dlclose(lib_handle);
    return input;
  }

  ffi_call(func_ptr, &cif, &result, args);

  // Convert result back to sm_object* based on return type
  sm_object *result_obj = create_sm_object(result_type, result);
  dlclose(lib_handle);
  return result_obj;
}
