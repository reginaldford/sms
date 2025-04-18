// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"

// Globals from sm_global.c
extern sm_heap_set *sms_all_heaps;
extern sm_heap     *sms_heap;
extern sm_heap     *sms_other_heap;
extern sm_heap     *sms_symbol_heap;
extern sm_heap     *sms_symbol_name_heap;
extern sm_symbol   *sms_true;
extern sm_symbol   *sms_false;
extern sm_stack2   *sms_stack;
extern sm_stack2   *sms_cx_stack;
extern sm_object *(*sm_add_functions[])();

// Basic type checking

// Evaluate the argument, then run type check
static inline sm_object *eager_type_check(sm_expr *sme, uint32_t operand, uint32_t param_type) {
  sm_object *obj = sm_eval(sm_expr_get_arg(sme, operand));
  if (param_type != obj->my_type) {
    sm_string *source  = (sm_string *)sm_cx_get(sme->notes, sm_new_symbol("source", 6));
    sm_f64    *line    = (sm_f64 *)sm_cx_get(sme->notes, sm_new_symbol("line", 4));
    sm_string *message = sm_new_fstring_at(
      sms_heap, "Wrong type for argument %i on %s. Argument type is: %s , but Expected: %s (%s:%i)",
      operand, sm_global_fn_name(sme->op), sm_type_name(obj->my_type), sm_type_name(param_type),
      __FILE__, __LINE__);
    sm_error *err = sm_new_error(12, "typeMismatch", message->size, &message->content, source->size,
                                 &source->content, (uint32_t)line->value);
    if (param_type == SM_ERR_TYPE) {
      err->origin = err;
    }
    return ((sm_object *)err);
  }
  return (sm_object *)obj;
}

// Evaluate the argument, then run type check. 2 possibilities allowed
static inline sm_object *eager_type_check2(sm_expr *sme, uint32_t operand, uint32_t param_type1,
                                           uint32_t param_type2) {
  sm_object *obj = sm_eval(sm_expr_get_arg(sme, operand));
  if (param_type1 != obj->my_type && param_type2 != obj->my_type) {
    sm_string *source  = (sm_string *)sm_cx_get(sme->notes, sm_new_symbol("source", 6));
    sm_f64    *line    = (sm_f64 *)sm_cx_get(sme->notes, sm_new_symbol("line", 4));
    sm_string *message = sm_new_fstring_at(
      sms_heap, "Wrong type for argument %i on %s. Argument type is: %s , but Expected: %s or %s",
      operand, sm_global_fn_name(sme->op), sm_type_name(obj->my_type), sm_type_name(param_type1),
      sm_type_name(param_type2));
    sm_error *err = sm_new_error(12, "typeMismatch", message->size, &message->content, source->size,
                                 &source->content, (uint32_t)line->value);
    if (param_type1 == SM_ERR_TYPE || param_type2 == SM_ERR_TYPE) {
      err->origin = err;
    }
    return ((sm_object *)err);
  }
  return (obj);
}

// Evaluate the argument, then run type check. 3 possibilities allowed
static inline sm_object *eager_type_check3(sm_expr *sme, uint32_t operand, uint32_t param_type1,
                                           uint32_t param_type2, uint32_t param_type3) {
  sm_object *obj = sm_eval(sm_expr_get_arg(sme, operand));
  if (param_type1 != obj->my_type && param_type2 != obj->my_type && param_type3 != obj->my_type) {
    sm_string *source  = (sm_string *)sm_cx_get(sme->notes, sm_new_symbol("source", 6));
    sm_f64    *line    = (sm_f64 *)sm_cx_get(sme->notes, sm_new_symbol("line", 4));
    sm_string *message = sm_new_fstring_at(
      sms_heap,
      "Wrong type for argument %i on %s. Argument type is: %s "
      ", but Expected: %s, %s, or %s (%s:%i)",
      operand, sm_global_fn_name(sme->op), sm_type_name(obj->my_type), sm_type_name(param_type1),
      sm_type_name(param_type2), sm_type_name(param_type3), __FILE__, __LINE__);
    sm_error *err = sm_new_error(12, "typeMismatch", message->size, &message->content, source->size,
                                 &source->content, (uint32_t)line->value);
    if (param_type1 == SM_ERR_TYPE || param_type2 == SM_ERR_TYPE || param_type3 == SM_ERR_TYPE) {
      err->origin = err;
    }
    return ((sm_object *)err);
  }
  return (obj);
}
// Execute a function
sm_object *execute_fun(sm_fun *fun) {
  sm_expr *sf; // TODO: this would come from the stack
  sm_cx   *current_cx;
  switch (fun->my_type) {
  case SM_FUN_TYPE: {
    sm_object *content = fun->content;
    sm_object *result;
    // TODO: cx gets pushed to cx stack
    // sm_cx *new_cx = sm_new_cx(fun->parent);
    if (content->my_type == SM_EXPR_TYPE && ((sm_expr *)content)->op == SM_BLOCK_EXPR) {
      sm_expr *content_sme = (sm_expr *)fun->content;
      for (uint32_t i = 1; i < content_sme->size; i++) {
        result = sm_eval(sm_expr_get_arg(content_sme, i));
        if (result->my_type == SM_RETURN_TYPE)
          return (((sm_return *)result)->address);
      }
      return (result);
    } else
      return sm_eval(content);
    break;
  }
  case SM_SO_FUN_TYPE: {
    sm_so_fun *f                       = (sm_so_fun *)fun;
    sm_object *(*ff)(sm_object *input) = f->function;
    sm_object *output                  = ff((sm_object *)sf);
    return (output);
    break;
  }
  case SM_FF_TYPE: {
    sm_ff *ff = (sm_ff *)fun;
    if (sf->size < ff->cif.nargs) {
      printf("!not enough arguments for ffi call.\n");
      return ((sm_object *)sms_false);
    }
    void *args[ff->cif.nargs];
    // fill in the arg ptrs
    for (size_t i = 0; i < ff->cif.nargs; i++) {
      ffi_type *current_type = ff->cif.arg_types[i];
      // We cannot use a switch statement here.
      // so we use a series of conditions
      if (current_type == &ffi_type_double) {
        // something doesnt work with this typecheck:
        // type_check(sf, i, SM_F64_TYPE);
        sm_object *current_arg = sm_expr_get_arg(sf, i);
        if (current_arg->my_type != SM_F64_TYPE) {
          printf("wrong type for arg %lu\n", i);
          return (sm_object *)sms_false;
        }
        sm_f64 *num = (sm_f64 *)sm_expr_get_arg(sf, i);
        args[i]     = &num->value;
      } else {
        printf("unsupported ffi type\n");
        return ((sm_object *)sms_false);
      }
    }

    double return_val = 0;
    ffi_call(&ff->cif, FFI_FN(ff->fptr), &return_val, args);
    return ((sm_object *)sm_new_f64(return_val));
    break;
  }
  default:
    return ((sm_object *)fun);
  }
  return ((sm_object *)fun);
}


sm_object *sm_eval(sm_object *input) {
  switch (input->my_type) {
  case SM_EXPR_TYPE: {
    sm_expr *sme = (sm_expr *)input;
    uint32_t op  = sme->op;
    switch (op) {
    case SM_VERSION_EXPR: {
      return (((sm_object *)sms_global_version()));
      break;
    }
    case SM_NEW_F64_EXPR: {
      // TODO stack push to sms_stack every such var:
      sm_object *fromObj = eager_type_check3(sme, 0, SM_F64_TYPE, SM_UI8_TYPE, SM_STRING_TYPE);
      switch (fromObj->my_type) {
      case SM_F64_TYPE:
        return (((sm_object *)sm_new_f64(((sm_f64 *)fromObj)->value)));
      case SM_UI8_TYPE:
        return (((sm_object *)sm_new_f64(((sm_ui8 *)fromObj)->value)));
      case SM_STRING_TYPE: {
        char       *endptr;
        const char *str_content = &((sm_string *)fromObj)->content;
        double      value       = strtod(str_content, &endptr);
        // Check for conversion errors
        if (endptr == str_content) {
          sm_symbol *title = sm_new_symbol("cannotConvertToF64", 18);
          sm_string *message =
            sm_new_fstring_at(sms_heap, "Cannot convert string to f64: %s", str_content);
          return (((sm_object *)sm_new_error_from_expr(title, message, sme, NULL)));
        }
        return (((sm_object *)sm_new_f64(value)));
      }
      default: {
        sm_symbol *title   = sm_new_symbol("cannotConvertToF64", 18);
        sm_string *message = sm_new_fstring_at(sms_heap, "Cannot convert object of type %s to f64.",
                                               sm_type_name(fromObj->my_type));
        sm_error  *err     = sm_new_error_from_expr(title, message, sme, NULL);
        if (fromObj->my_type == SM_ERR_TYPE)
          err->origin = (sm_error *)fromObj;
        return (sm_object *)err;
      }
      }
    }
    case SM_NEW_UI8_EXPR: {
      sm_object *fromObj = eager_type_check3(sme, 0, SM_UI8_TYPE, SM_F64_TYPE, SM_STRING_TYPE);
      if (fromObj->my_type == SM_ERR_TYPE)
        return (fromObj);
      switch (fromObj->my_type) {
      case SM_F64_TYPE:
        return (((sm_object *)sm_new_ui8(((sm_f64 *)fromObj)->value)));
      case SM_UI8_TYPE:
        return (((sm_object *)sm_new_ui8(((sm_ui8 *)fromObj)->value)));
      case SM_STRING_TYPE: {
        char       *endptr;
        const char *str_content = &((sm_string *)fromObj)->content;
        double      value       = strtod(str_content, &endptr);
        // Check for conversion errors
        if (endptr == str_content) {
          sm_symbol *title = sm_new_symbol("cannotConvertToUi8", 18);
          sm_string *message =
            sm_new_fstring_at(sms_heap, "Cannot convert string to ui8: %s", str_content);
          return (((sm_object *)sm_new_error_from_expr(title, message, sme, NULL)));
        }
        return (((sm_object *)sm_new_ui8((char)value)));
      }
      default: {
        sm_symbol *title   = sm_new_symbol("cannotConvertToUI8", 18);
        sm_string *message = sm_new_fstring_at(sms_heap, "Cannot convert object of type %s to ui8.",
                                               sm_type_name(fromObj->my_type));
        return (((sm_object *)sm_new_error_from_expr(title, message, sme, NULL)));
      }
      }
    }
    case SM_DATE_EXPR: {
      time_t rawtime;
      time(&rawtime);
      struct tm *timeinfo   = localtime(&rawtime);
      uint32_t  *time_tuple = (uint32_t *)timeinfo;
      sm_space  *space      = sm_new_space(sizeof(f64) * 9);
      sm_array  *result     = sm_new_array(SM_F64_TYPE, 9, (sm_object *)space, 0);
      for (uint32_t i = 0; i < 9; i++)
        sm_f64_array_set(result, i, time_tuple[i]);
      return (sm_object *)result;
      break;
    }
    case SM_HEAPSAVE_EXPR: {
      // Obtain the file name using eager_type_check
      sm_string *fname_str = (sm_string *)eager_type_check(sme, 0, SM_STRING_TYPE);
      if (fname_str->my_type == SM_ERR_TYPE)
        return (((sm_object *)fname_str)); // return(the error if type check fails);
      char *fname_cstr = &(fname_str->content);
      return ((sm_object *)sm_new_f64(sm_mem_dump(sms_heap, fname_cstr)));
    }
    case SM_IMAGESAVE_EXPR: {
      // Obtain the file name using eager_type_check
      sm_string *fname_str = (sm_string *)eager_type_check(sme, 0, SM_STRING_TYPE);
      if (fname_str->my_type == SM_ERR_TYPE)
        return (((sm_object *)fname_str)); // return(the error if type check fails);

      char *fname_cstr = &(fname_str->content);

      // Create and populate the sm_img structure with the current globals
      sm_img img;
      img.env                  = *sm_global_environment(NULL); // Copy the current environment
      img.sms_heap             = *sms_heap;                    // Copy the main heap
      img.sms_symbol_heap      = *sms_symbol_heap;             // Copy the symbol heap
      img.sms_symbol_name_heap = *sms_symbol_name_heap;        // Copy the symbol name heap

      // Perform garbage collection to compact the sms_heap
      sm_heap *compacted_heap = sm_new_heap(sms_heap->capacity, true); // Create a new heap for GC
      // TODO: sm_heap_compact_to(from,to)
      // sm_garbage_collect(sms_heap, compacted_heap); // Compact the main heap uint32_to
      // the new one

      // Open the file for writing
      FILE *file = fopen(fname_cstr, "wb");
      if (!file) {
        fprintf(stderr, "Failed to open file for writing: %s\n", fname_cstr);
        sm_symbol *title   = sm_new_symbol("imgSaveFailed", 13);
        sm_string *message = sm_new_string(36, "Failed to open for saving the image.");
        return ((sm_object *)sm_new_error_from_expr(title, message, sme, NULL));
      }

      // Write the sm_img structure to the file
      if (fwrite(&img, sizeof(sm_img), 1, file) != 1) {
        fclose(file);
        fprintf(stderr, "Failed to write image to file: %s\n", fname_cstr);
        sm_symbol *title   = sm_new_symbol("imgSaveFailed", 13);
        sm_string *message = sm_new_string(26, "Failed to write the image.");
        return ((sm_object *)sm_new_error_from_expr(title, message, sme, NULL));
      }

      // Write the sm_heap_set to the file
      if (fwrite(sms_all_heaps, sizeof(sm_heap_set), 1, file) != 1) {
        fclose(file);
        fprintf(stderr, "Failed to write heaps to file: %s\n", fname_cstr);
        sm_symbol *title   = sm_new_symbol("heapSaveFailed", 14);
        sm_string *message = sm_new_string(27, "Failed to write heaps data.");
        return ((sm_object *)sm_new_error_from_expr(title, message, sme, NULL));
      }

      // Write the compacted sms_heap's original start point and compacted storage to the
      // file
      if (fwrite(&sms_heap->storage, sizeof(char *), 1, file) != 1 ||
          fwrite(compacted_heap->storage, compacted_heap->used, 1, file) != 1) {
        fclose(file);
        free(compacted_heap); // Free the heap using C's free function
        fprintf(stderr, "Failed to write heap data to file: %s\n", fname_cstr);
        sm_symbol *title   = sm_new_symbol("heapSaveFailed", 14);
        sm_string *message = sm_new_string(31, "Failed to write compacted heap data.");
        return ((sm_object *)sm_new_error_from_expr(title, message, sme, NULL));
      }

      // Clean up
      fclose(file);
      free(compacted_heap); // Free the compacted heap after writing

      // Return sms_true to indicate success
      return ((sm_object *)sms_true);
    }
    case SM_GC_EXPR: {
      sm_garbage_collect();
      return ((sm_object *)sms_true);
      break;
    }
    case SM_SLEEP_EXPR: {
      sm_f64 *obj0 = (sm_f64 *)eager_type_check(sme, 0, SM_F64_TYPE);
      if (obj0->my_type != SM_F64_TYPE) {
        sm_symbol *title   = sm_new_symbol("nonNumericTime", 14);
        sm_string *message = sm_new_string(48, "sleep function was provided a non-numeric value.");
        return (((sm_object *)sm_new_error_from_expr(title, message, sme, NULL)));
      }
      uint32_t tms;
      tms = (uint32_t)((sm_f64 *)obj0)->value;
      if (tms < 0) {
        sm_symbol *title   = sm_new_symbol("negativeTime", 12);
        sm_string *message = sm_new_string(45, "sleep function was provided a negative value.");
        return (((sm_object *)sm_new_error_from_expr(title, message, sme, NULL)));
      }
      struct timespec ts;
      uint32_t        ret;
      ts.tv_sec  = tms / 1000;
      ts.tv_nsec = (tms % 1000) * 1000000;
      do {
        ret = nanosleep(&ts, &ts);
      } while (ret);
      return (((sm_object *)sms_true));
      break;
    }
    case SM_FORK_EXPR: {
      pid_t pid = fork();
      return (((sm_object *)sm_new_f64(pid)));
    }
    case SM_WAIT_EXPR: {
      int32_t  status;
      uint32_t child_pid = wait(&status);
      status             = WEXITSTATUS(status);
      if (child_pid == -1)
        status = 1;
      return ((sm_object *)sm_new_expr_2(SM_TUPLE_EXPR, (sm_object *)sm_new_f64(child_pid),
                                         (sm_object *)sm_new_f64(status), NULL));
    }
    case SM_EXEC_EXPR: {
      sm_string *path = (sm_string *)eager_type_check(sme, 0, SM_STRING_TYPE);
      if (path->my_type == SM_ERR_TYPE)
        return (((sm_object *)path));
      // The system command leaves 8 bits for extra information
      // We do not need it, so we shift away the 8 bits
      uint32_t result = system(&path->content) >> 8;
      return (((sm_object *)sm_new_f64(result)));
      break;
    }
    case SM_EXECTOSTR_EXPR: {
      sm_string *path = (sm_string *)eager_type_check(sme, 0, SM_STRING_TYPE);
      if (path->my_type == SM_ERR_TYPE)
        return (((sm_object *)path));
      FILE  *fp;
      char   buffer[128]; // Buffer size to read the command output in chunks
      char  *output_data = NULL;
      size_t total_size  = 0;
      // Execute the command and open a pipe to read its output
      fp = popen(&(path->content), "r");
      if (fp == NULL) {
        sm_string *message = sm_new_string(45, "Failed to open pipe for command execution.");
        sm_symbol *title   = sm_new_symbol("osExecToStrPopenFailed", 11);
        return (((sm_object *)sm_new_error_from_expr(title, message, sme, NULL)));
      }
      // Read the command output in chunks
      while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        size_t buffer_len      = strlen(buffer);
        char  *new_output_data = realloc(output_data, total_size + buffer_len + 1);
        if (new_output_data == NULL) {
          free(output_data);
          sm_string *message = sm_new_string(47, "Failed to allocate memory for command output.");
          sm_symbol *title   = sm_new_symbol("osExecToStrMemOverFlow", 11);
          return (((sm_object *)sm_new_error_from_expr(title, message, sme, NULL)));
        }
        output_data = new_output_data;
        memcpy(output_data + total_size, buffer, buffer_len);
        total_size += buffer_len;
        output_data[total_size] = '\0'; // Null terminate
      }
      // Close the pipe and get the command exit status
      uint32_t return_code = pclose(fp) >> 8;
      // Create a new sm_string with the collected output and free the output data
      sm_string *result_str = sm_new_string(total_size, output_data);
      free(output_data);
      // Return a size 2 tuple with return value and return string
      sm_f64  *output_code = sm_new_f64(return_code);
      sm_expr *output =
        sm_new_expr_2(SM_TUPLE_EXPR, (sm_object *)output_code, (sm_object *)result_str, NULL);
      return (((sm_object *)output));
      break;
    }

    case SM_OS_GETENV_EXPR: {
      sm_string *key = (sm_string *)eager_type_check(sme, 0, SM_STRING_TYPE);
      if (key->my_type == SM_ERR_TYPE)
        return (((sm_object *)key));
      char *result = getenv(&key->content);
      if (!result) {
        sm_symbol *title = sm_new_symbol("osGetEnvFailed", 14);
        sm_string *message =
          sm_new_fstring_at(sms_heap, "Failed to get environment variable: %s", &key->content);
        return (((sm_object *)sm_new_error_from_expr(title, message, sme, NULL)));
      }
      return (((sm_object *)sm_new_string(strlen(result), result)));
      break;
    }
    case SM_OS_SETENV_EXPR: {
      sm_string *key = (sm_string *)eager_type_check(sme, 0, SM_STRING_TYPE);
      if (key->my_type == SM_ERR_TYPE)
        return (((sm_object *)key));
      sm_string *value = (sm_string *)eager_type_check(sme, 1, SM_STRING_TYPE);
      if (value->my_type == SM_ERR_TYPE)
        return (((sm_object *)value));
      uint32_t result = setenv(&key->content, &value->content, 1);
      return (((sm_object *)sm_new_f64(result)));
      break;
    }
    case SM_LS_EXPR: {
      DIR           *dir;
      struct dirent *entry;
      struct stat    file_stat;
      const uint32_t MAX_ENTRIES = 1024;
      sm_string     *entry_names[MAX_ENTRIES];
      bool           entry_types[MAX_ENTRIES];
      uint32_t       i = 0;

      char cwd[1024];
      if (getcwd(cwd, sizeof(cwd))) {
      } else {
        sm_string *msg =
          sm_new_fstring_at(sms_heap, "Error: Current working directory is invalid: %s .\n", cwd);
        sm_symbol *title = sm_new_symbol("invalidWorkingDirectory", 23);
        return (((sm_object *)sm_new_error_from_expr(title, msg, sme, NULL)));
      }

      dir = opendir(cwd);
      if (dir == NULL) {
        sm_string *msg =
          sm_new_fstring_at(sms_heap, "Error: Current working directory is invalid: %s .\n", cwd);
        sm_symbol *title = sm_new_symbol("invalidWorkingDirectory", 23);
        return (((sm_object *)sm_new_error_from_expr(title, msg, sme, NULL)));
      }
      while ((entry = readdir(dir)) && i < MAX_ENTRIES) {
        uint32_t path_length = strlen(cwd) + strlen(entry->d_name) + 1;
        char     full_path[path_length + 1];
        snprintf(full_path, path_length + 1, "%s/%s", cwd, entry->d_name);
        stat(full_path, &file_stat);
        entry_names[i] = sm_new_string(strlen(entry->d_name), entry->d_name);
        // S_ISDIR returns nonzero iff it's a directory
        entry_types[i] = S_ISDIR(file_stat.st_mode) != 0;
        i++;
      }
      closedir(dir);
      sm_expr *names_arr = sm_new_expr_n(SM_TUPLE_EXPR, i, i, NULL);
      for (uint32_t names_i = 0; names_i < i; names_i++) {
        sm_expr_set_arg(names_arr, names_i, (sm_object *)entry_names[names_i]);
      }
      return ((sm_object *)names_arr);
      break;
    }
    case SM_PWD_EXPR: {
      char cwd[1024];
      if (getcwd(cwd, sizeof(cwd))) {
      } else {
        sm_string *msg =
          sm_new_fstring_at(sms_heap, "Error: Current working directory is invalid: %s .\n", cwd);
        sm_symbol *title = sm_new_symbol("pwdFailed", 9);
        return (((sm_object *)sm_new_error_from_expr(title, msg, sme, NULL)));
      }
      return (((sm_object *)sm_new_string(strlen(cwd), cwd)));
      break;
    }
    case SM_CD_EXPR: {
      sm_string *path = (sm_string *)eager_type_check(sme, 0, SM_STRING_TYPE);
      if (path->my_type != SM_STRING_TYPE)
        return (((sm_object *)path));
      char *path_cstr = &path->content;
      if (chdir(path_cstr) != 0) {
        sm_symbol *title = sm_new_symbol("cdFailed", 8);
        sm_string *message =
          sm_new_fstring_at(sms_heap, "Failed to change directory to %s", path_cstr);
        return (((sm_object *)sm_new_error_from_expr(title, message, sme, NULL)));
      }
      return (((sm_object *)sms_true));
      break;
    }
    case SM_DATE_STR_EXPR: {
      time_t     rawtime;
      struct tm *timeinfo;
      time(&rawtime);
      timeinfo          = localtime(&rawtime);
      sm_string *result = sm_new_string_manual(24);
      sm_strncpy(&(result->content), asctime(timeinfo), 24);
      return (((sm_object *)result));
      break;
    }
    case SM_TIME_EXPR: {
      static struct timeval t;
      gettimeofday(&t, NULL);
      sm_space *space  = NULL;
      sm_array *result = NULL;
      space            = sm_new_space(sizeof(f64) * 2);
      result           = sm_new_array(SM_F64_TYPE, 2, (sm_object *)space, 0);
      sm_f64_array_set(result, 0, t.tv_sec);
      sm_f64_array_set(result, 1, t.tv_usec);
      return (((sm_object *)result));
      break;
    }
    case SM_STR_FIND_EXPR: {
      sm_string *haystack = (sm_string *)eager_type_check(sme, 0, SM_STRING_TYPE);
      if (haystack->my_type == SM_ERR_TYPE)
        return (((sm_object *)haystack));
      sm_string *needle = (sm_string *)eager_type_check(sme, 1, SM_STRING_TYPE);
      if (needle->my_type == SM_ERR_TYPE)
        return (((sm_object *)needle));
      return ((sm_str_find(haystack, needle)));
      break;
    }
    case SM_STR_FINDR_EXPR: {
      sm_string *haystack = (sm_string *)eager_type_check(sme, 0, SM_STRING_TYPE);
      if (haystack->my_type == SM_ERR_TYPE)
        return (((sm_object *)haystack));
      sm_string *needle = (sm_string *)eager_type_check(sme, 1, SM_STRING_TYPE);
      if (needle->my_type == SM_ERR_TYPE)
        return (((sm_object *)needle));
      return ((sm_str_findr(haystack, needle)));
      break;
    }
    case SM_STR_SPLIT_EXPR: {
      sm_string *haystack = (sm_string *)eager_type_check(sme, 0, SM_STRING_TYPE);
      if (haystack->my_type == SM_ERR_TYPE)
        return (((sm_object *)haystack));
      sm_string *needle = (sm_string *)eager_type_check(sme, 1, SM_STRING_TYPE);
      if (needle->my_type == SM_ERR_TYPE)
        return (((sm_object *)needle));
      return (((sm_object *)sm_str_split(haystack, needle)));
      break;
    }
    case SM_STR_PART_EXPR: {
      sm_string *str0 = (sm_string *)eager_type_check(sme, 0, SM_STRING_TYPE);
      if (str0->my_type == SM_ERR_TYPE)
        return (((sm_object *)str0));
      sm_f64 *start = (sm_f64 *)eager_type_check(sme, 1, SM_F64_TYPE);
      if (start->my_type == SM_ERR_TYPE)
        return (((sm_object *)start));
      sm_f64 *len = (sm_f64 *)eager_type_check(sme, 2, SM_F64_TYPE);
      if (len->my_type == SM_ERR_TYPE)
        return (((sm_object *)len));
      if (start->value < 0 || start->value >= str0->size) {
        sm_symbol *title   = sm_new_symbol("strPartIndexErr", 15);
        sm_string *message = sm_new_fstring_at(
          sms_heap, "Calling strPart with out of range start value: %i", (uint32_t)start->value);
        return (((sm_object *)sm_new_error_from_expr(title, message, sme, NULL)));
      }
      if (len->value > str0->size - start->value) {
        sm_symbol *title   = sm_new_symbol("strPartLengthErr", 16);
        sm_string *message = sm_new_fstring_at(
          sms_heap, "Calling strPart with out of range length value: %i", (uint32_t)len->value);
        return (((sm_object *)sm_new_error_from_expr(title, message, sme, NULL)));
      }
      sm_string *new_str = sm_new_string_manual((uint32_t)len->value);
      char      *content = &(new_str->content);
      sm_strncpy(content, &(str0->content) + (uint32_t)start->value, (uint32_t)len->value);
      return (((sm_object *)new_str));
      break;
    }
    case SM_STR_MUT_EXPR: {
      sm_string *original_str = (sm_string *)eager_type_check(sme, 0, SM_STRING_TYPE);
      if (original_str->my_type == SM_ERR_TYPE)
        return (((sm_object *)original_str));
      sm_f64 *start_index = (sm_f64 *)eager_type_check(sme, 1, SM_F64_TYPE);
      if (start_index->my_type == SM_ERR_TYPE)
        return (((sm_object *)start_index));
      sm_string *replacement_str = (sm_string *)eager_type_check(sme, 2, SM_STRING_TYPE);
      if (replacement_str->my_type == SM_ERR_TYPE)
        return (((sm_object *)replacement_str));
      // Check if new size exceeds original size
      if (original_str->size - (uint32_t)start_index->value < (uint32_t)replacement_str->size) {
        sm_symbol *title   = sm_new_symbol("strMutOverflow", 14);
        sm_string *message = sm_new_string(46, "Could not mutate string within the size limit.");
        return (((sm_object *)sm_new_error_from_expr(title, message, sme, NULL)));
      }
      char *content = &(original_str->content);
      // Copy replacement string uint32_to the original string
      sm_strncpy_unsafe(content + (uint32_t)start_index->value, &(replacement_str->content),
                        replacement_str->size);
      // No need to copy the remainder since the string size stays the same
      return (((sm_object *)original_str));
      break;
    }
    case SM_STR_CAT_EXPR: {
      sm_string *str0 = (sm_string *)eager_type_check(sme, 0, SM_STRING_TYPE);
      if (str0->my_type != SM_STRING_TYPE)
        return (((sm_object *)str0));
      sm_string *str1 = (sm_string *)eager_type_check(sme, 1, SM_STRING_TYPE);
      if (str1->my_type != SM_STRING_TYPE)
        return ((sm_object *)str1);
      uint32_t   s0      = str0->size;
      uint32_t   s1      = str1->size;
      sm_string *new_str = sm_new_string_manual(s0 + s1);
      char      *content = &(new_str->content);
      sm_strncpy_unsafe(content, &(str0->content), str0->size);
      sm_strncpy(content + s0, &(str1->content), s1);
      return ((sm_object *)new_str);
      break;
    }
    case SM_STR_SIZE_EXPR: {
      sm_string *str0 = (sm_string *)eager_type_check(sme, 0, SM_STRING_TYPE);
      if (str0->my_type == SM_ERR_TYPE)
        return (((sm_object *)str0));
      return (((sm_object *)sm_new_f64(str0->size)));
      break;
    }
    case SM_STR_REPEAT_EXPR: {
      sm_string *str = (sm_string *)eager_type_check(sme, 0, SM_STRING_TYPE);
      if (str->my_type == SM_ERR_TYPE)
        return (((sm_object *)str));
      sm_f64 *reps = (sm_f64 *)eager_type_check(sme, 1, SM_F64_TYPE);
      if (reps->my_type == SM_ERR_TYPE)
        return (((sm_object *)reps));
      f64        repetitions   = reps->value;
      uint32_t   original_size = str->size;
      uint32_t   new_size      = (uint32_t)(original_size * repetitions);
      sm_string *new_str       = sm_new_string_manual(new_size);
      char      *content       = &(new_str->content);
      for (int i = 0; i < new_size; i += original_size) {
        sm_strncpy(content + i, &(str->content), original_size);
      }
      return (((sm_object *)new_str));
      break;
    }
    case SM_ZEROS_EXPR: {
      sm_f64 *num0 = (sm_f64 *)eager_type_check(sme, 0, SM_F64_TYPE);
      if (num0->my_type != SM_F64_TYPE)
        return (((sm_object *)num0));
      if (num0->value < 1)
        return (((sm_object *)sm_new_expr_0(SM_TUPLE_EXPR, NULL)));
      sm_f64  *zero   = sm_new_f64(0);
      sm_expr *output = sm_new_expr_n(SM_TUPLE_EXPR, (uint32_t)num0->value, (int)num0->value, NULL);
      for (int i = 0; i < num0->value; i++)
        sm_expr_set_arg(output, i, (sm_object *)zero);
      return (((sm_object *)output));
      break;
    }

    case SM_PART_EXPR: {
      sm_expr *list0 = (sm_expr *)eager_type_check(sme, 0, SM_EXPR_TYPE);
      if (list0->my_type == SM_ERR_TYPE)
        return (((sm_object *)list0));
      sm_f64 *start = (sm_f64 *)eager_type_check(sme, 1, SM_F64_TYPE);
      if (start->my_type == SM_ERR_TYPE)
        return (((sm_object *)start));
      sm_f64 *len = (sm_f64 *)eager_type_check(sme, 2, SM_F64_TYPE);
      if (len->my_type == SM_ERR_TYPE)
        return (((sm_object *)len));
      if (start->value < 0 || start->value >= list0->size) {
        sm_symbol *title   = sm_new_symbol("partIndexErr", 12);
        sm_string *message = sm_new_fstring_at(
          sms_heap, "Calling part with out of range start value: %i", (uint32_t)start->value);
        return (((sm_object *)sm_new_error_from_expr(title, message, sme, NULL)));
      }
      if (len->value > list0->size - start->value) {
        sm_symbol *title   = sm_new_symbol("partLengthErr", 13);
        sm_string *message = sm_new_fstring_at(
          sms_heap, "Calling part with out of range length value: %i", (uint32_t)len->value);
        return (((sm_object *)sm_new_error_from_expr(title, message, sme, NULL)));
      }
      sm_expr *new_list = sm_new_expr_n(SM_TUPLE_EXPR, (uint32_t)len->value, (int)len->value, NULL);
      for (int i = 0; i < (uint32_t)len->value; i++) {
        sm_object *element = sm_expr_get_arg(list0, (uint32_t)start->value + i);
        sm_expr_set_arg(new_list, i, element);
      }
      return (((sm_object *)new_list));
      break;
    }
    case SM_UI8_REPEAT_EXPR: {
      sm_expr *obj = (sm_expr *)eager_type_check2(sme, 0, SM_EXPR_TYPE, SM_ARRAY_TYPE);
      if (obj->my_type == SM_ERR_TYPE)
        return (((sm_object *)obj));
      sm_f64 *repeat_count_obj = (sm_f64 *)eager_type_check(sme, 1, SM_F64_TYPE);
      if (repeat_count_obj->my_type == SM_ERR_TYPE)
        return (((sm_object *)repeat_count_obj));
      uint32_t repeat_count = (uint32_t)repeat_count_obj->value;
      if (repeat_count == 0)
        return (((sm_object *)sm_new_array(SM_UI8_TYPE, 0, NULL, sizeof(sm_space))));
      uint32_t  total_size = 0;
      sm_space *new_space  = NULL;
      ui8      *dst_data   = NULL;
      switch (obj->my_type) {
      case SM_EXPR_TYPE: {
        sm_expr *arr = (sm_expr *)obj;
        total_size   = arr->size * repeat_count;
        new_space    = sm_new_space(total_size);
        dst_data     = (ui8 *)(new_space + 1);
        for (uint32_t i = 0; i < repeat_count; i++) {
          for (uint32_t j = 0; j < arr->size; j++) {
            sm_object *element = sm_expr_get_arg(arr, j);
            switch (element->my_type) {
            case SM_F64_TYPE:
              dst_data[i * arr->size + j] = (ui8)((sm_f64 *)element)->value;
              break;
            case SM_UI8_TYPE:
              dst_data[i * arr->size + j] = ((sm_ui8 *)element)->value;
              break;
            case SM_STRING_TYPE:
              dst_data[i * arr->size + j] =
                (ui8)(((sm_string *)element)->content); // Convert first character
              break;
            default: {
              sm_symbol *title   = sm_new_symbol("InvalidElementType", 17);
              sm_string *message = sm_new_fstring_at(
                sms_heap, "Unsupported element type %i in tuple", (uint32_t)element->my_type);
              return (((sm_object *)sm_new_error_from_expr(title, message, sme, NULL)));
            }
            }
          }
        }
        break;
      }
      case SM_ARRAY_TYPE: {
        sm_array *arr = (sm_array *)obj;
        total_size    = arr->size * repeat_count;
        new_space     = sm_new_space(total_size);
        dst_data      = (ui8 *)(new_space + 1);
        switch (arr->inner_type) {
        case SM_UI8_TYPE: {
          ui8 *src_data = sm_ui8_array_get_start(arr);
          for (uint32_t i = 0; i < repeat_count; i++)
            memcpy(dst_data + i * arr->size, src_data, arr->size);
          break;
        }
        case SM_F64_TYPE: {
          f64 *src_data = sm_f64_array_get_start(arr);
          for (uint32_t i = 0; i < repeat_count; i++)
            for (uint32_t j = 0; j < arr->size; j++)
              dst_data[i * arr->size + j] = (ui8)src_data[j];
          break;
        }
        default:
          return ((sm_object *)sm_new_error_from_strings(
            sm_new_symbol("InvalidType", 11),
            sm_new_string(29, "Unsupported array type for ui8 repeat"), NULL, 0, NULL));
        }
        break;
      }
      default:
        return ((sm_object *)sm_new_error_from_strings(
          sm_new_symbol("InvalidType", 11), sm_new_string(26, "Invalid type for ui8 repeat"), NULL,
          0, NULL));
      }
      return ((sm_object *)sm_new_array(SM_UI8_TYPE, total_size, (sm_object *)new_space,
                                        sizeof(sm_space)));
    }
    case SM_F64_REPEAT_EXPR: {
      sm_expr *obj = (sm_expr *)eager_type_check2(sme, 0, SM_EXPR_TYPE, SM_ARRAY_TYPE);
      if (obj->my_type == SM_ERR_TYPE)
        return (((sm_object *)obj));
      sm_f64 *repeat_count_obj = (sm_f64 *)eager_type_check(sme, 1, SM_F64_TYPE);
      if (repeat_count_obj->my_type == SM_ERR_TYPE)
        return (((sm_object *)repeat_count_obj));
      uint32_t repeat_count = (uint32_t)repeat_count_obj->value;

      if (repeat_count == 0)
        return (((sm_object *)sm_new_array(SM_F64_TYPE, 0, NULL, sizeof(sm_space))));

      uint32_t  total_size = 0;
      sm_space *new_space  = NULL;
      f64      *dst_data   = NULL;

      switch (obj->my_type) {
      case SM_EXPR_TYPE: {
        sm_expr *arr = (sm_expr *)obj;
        total_size   = arr->size * repeat_count;
        new_space    = sm_new_space(total_size * sizeof(f64));
        dst_data     = (f64 *)(new_space + 1);
        for (uint32_t i = 0; i < repeat_count; i++)
          for (uint32_t j = 0; j < arr->size; j++)
            dst_data[i * arr->size + j] = (f64)(uintptr_t)sm_expr_get_arg(arr, j);
        break;
      }
      case SM_ARRAY_TYPE: {
        sm_array *arr = (sm_array *)obj;
        total_size    = arr->size * repeat_count;
        new_space     = sm_new_space(total_size * sizeof(f64));
        dst_data      = (f64 *)(new_space + 1);
        switch (arr->inner_type) {
        case SM_UI8_TYPE: {
          ui8 *src_data = sm_ui8_array_get_start(arr);
          for (uint32_t i = 0; i < repeat_count; i++)
            for (uint32_t j = 0; j < arr->size; j++)
              dst_data[i * arr->size + j] = (f64)src_data[j];
          break;
        }
        case SM_F64_TYPE: {
          f64 *src_data = sm_f64_array_get_start(arr);
          for (uint32_t i = 0; i < repeat_count; i++)
            memcpy(dst_data + i * arr->size, src_data, arr->size * sizeof(f64));
          break;
        }
        default:
          return ((sm_object *)sm_new_error_from_strings(
            sm_new_symbol("InvalidType", 11),
            sm_new_string(29, "Unsupported array type for f64 repeat"), NULL, 0, NULL));
        }
        break;
      }
      default:
        return ((sm_object *)sm_new_error_from_strings(
          sm_new_symbol("InvalidType", 11), sm_new_string(26, "Invalid type for f64 repeat"), NULL,
          0, NULL));
      }

      return ((sm_object *)sm_new_array(SM_F64_TYPE, total_size, (sm_object *)new_space,
                                        sizeof(sm_space)));
    }
    case SM_CAT_EXPR: {
      sm_expr *list0 = (sm_expr *)eager_type_check(sme, 0, SM_EXPR_TYPE);
      if (list0->my_type == SM_ERR_TYPE)
        return (((sm_object *)list0));
      sm_expr *list1 = (sm_expr *)eager_type_check(sme, 1, SM_EXPR_TYPE);
      if (list1->my_type == SM_ERR_TYPE)
        return (((sm_object *)list1));
      uint32_t size0     = list0->size;
      uint32_t size1     = list1->size;
      uint32_t new_size  = size0 + size1;
      sm_expr *new_tuple = sm_new_expr_n(SM_TUPLE_EXPR, new_size, new_size, NULL);
      for (size_t i = 0; i < size0; i++) {
        sm_object *element = sm_expr_get_arg(list0, i);
        sm_expr_set_arg(new_tuple, i, element);
      }
      for (size_t i = 0; i < size1; i++) {
        sm_object *element = sm_expr_get_arg(list1, i);
        sm_expr_set_arg(new_tuple, size0 + i, element);
      }
      return (((sm_object *)new_tuple));
      break;
    }
    case SM_EXIT_EXPR: {
      uint32_t exit_code = 0;
      if (sme->size != 0) {
        sm_f64 *num0 = (sm_f64 *)eager_type_check2(sme, 0, SM_F64_TYPE, SM_UI8_TYPE);
        if (num0->my_type == SM_ERR_TYPE) {
          return (((sm_object *)num0));
        }
        // We have already verified the type class
        switch (num0->my_type) {
        case SM_UI8_TYPE: {
          sm_ui8 *byte_num = (sm_ui8 *)num0;
          exit_code        = (uint32_t)(byte_num->value);
        } break;
        case SM_F64_TYPE: {
          exit_code = (uint32_t)num0->value;
        } break;
        }
      }
      sm_signal_exit((uint32_t)exit_code);
      break;
    }
    case SM_LET_EXPR: {
      // Trust the parser for now regarding element 0 being a symbol
      sm_symbol *sym   = (sm_symbol *)sm_expr_get_arg(sme, 0);
      sm_object *value = sm_eval(sm_expr_get_arg(sme, 1));
      // If an error occurred, it is stored in the mapping
      // TODO: use cx stack!
      // sm_cx_let(current_cx, sym, value);
      return ((value));
    }
    case SM_CX_SETPARENT_EXPR: {
      sm_cx *cx = (sm_cx *)eager_type_check(sme, 0, SM_CX_TYPE);
      if (cx->my_type == SM_ERR_TYPE)
        return (((sm_object *)cx));
      sm_cx *new_parent = (sm_cx *)eager_type_check(sme, 1, SM_CX_TYPE);
      if (new_parent->my_type == SM_ERR_TYPE)
        return (((sm_object *)new_parent));
      cx         = (sm_cx *)sm_copy((sm_object *)cx);
      cx->parent = new_parent;
      return (((sm_object *)cx));
    }
    case SM_CX_LET_EXPR: {
      sm_cx *cx = (sm_cx *)eager_type_check(sme, 0, SM_CX_TYPE);
      if (cx->my_type == SM_ERR_TYPE)
        return (((sm_object *)cx));
      sm_symbol *sym = (sm_symbol *)eager_type_check(sme, 1, SM_SYMBOL_TYPE);
      if (sym->my_type == SM_ERR_TYPE)
        return (((sm_object *)sym));
      sm_object *value = (sm_object *)sm_eval(sm_expr_get_arg(sme, 2));
      sm_cx_let(cx, sym, value);
      return ((value));
    }

    case SM_CX_GET_EXPR: {
      sm_cx *cx = (sm_cx *)eager_type_check(sme, 0, SM_CX_TYPE);
      if (cx->my_type == SM_ERR_TYPE)
        return (((sm_object *)cx));
      sm_symbol *sym = (sm_symbol *)eager_type_check(sme, 1, SM_SYMBOL_TYPE);
      if (sym->my_type == SM_ERR_TYPE)
        return (((sm_object *)sym));
      sm_object *result = sm_cx_get(cx, sym);
      if (result)
        return ((result));
      sm_symbol *title = sm_new_symbol("cxGetFailed", 11);
      sm_string *message =
        sm_new_fstring_at(sms_heap, "cxGet did not find %s in this context.", &sym->name->content);
      return (((sm_object *)sm_new_error_from_expr(title, message, sme, NULL)));
    }
    case SM_CX_HAS_EXPR: {
      sm_cx *cx = (sm_cx *)eager_type_check(sme, 0, SM_CX_TYPE);
      if (cx->my_type == SM_ERR_TYPE)
        return (((sm_object *)cx));
      sm_symbol *sym = (sm_symbol *)eager_type_check(sme, 1, SM_SYMBOL_TYPE);
      if (sym->my_type == SM_ERR_TYPE)
        return (((sm_object *)sym));
      sm_object *result = sm_cx_get(cx, sym);
      if (result)
        return (((sm_object *)sms_true));
      return (((sm_object *)sms_false));
    }
    case SM_CX_GET_FAR_EXPR: {
      sm_cx *cx = (sm_cx *)eager_type_check(sme, 0, SM_CX_TYPE);
      if (cx->my_type == SM_ERR_TYPE)
        return (((sm_object *)cx));
      sm_symbol *sym = (sm_symbol *)eager_type_check(sme, 1, SM_SYMBOL_TYPE);
      if (sym->my_type == SM_ERR_TYPE)
        return (((sm_object *)sym));
      sm_object *result = sm_cx_get_far(cx, sym);
      if (!result)
        return (((sm_object *)sms_false));
      return ((result));
    }
    case SM_CX_HAS_FAR_EXPR: {
      sm_cx *cx = (sm_cx *)eager_type_check(sme, 0, SM_CX_TYPE);
      if (cx->my_type == SM_ERR_TYPE)
        return (((sm_object *)cx));
      sm_symbol *sym = (sm_symbol *)eager_type_check(sme, 1, SM_SYMBOL_TYPE);
      if (sym->my_type == SM_ERR_TYPE)
        return (((sm_object *)sym));
      sm_object *result = sm_cx_get_far(cx, sym);
      if (!result)
        return (((sm_object *)sms_false));
      return (((sm_object *)sms_true));
    }
    case SM_CX_SET_EXPR: {
      sm_cx *cx = (sm_cx *)eager_type_check(sme, 0, SM_CX_TYPE);
      if (cx->my_type == SM_ERR_TYPE)
        return (((sm_object *)cx));
      sm_symbol *sym = (sm_symbol *)eager_type_check(sme, 1, SM_SYMBOL_TYPE);
      if (sym->my_type == SM_ERR_TYPE)
        return (((sm_object *)sym));
      sm_object *value = sm_eval(sm_expr_get_arg(sme, 2));
      if (sm_cx_set(cx, sym, value))
        return (((sm_object *)sms_true));
      return (((sm_object *)sms_false));
    }
    case SM_CX_CLEAR_EXPR: {
      sm_cx *cx = (sm_cx *)eager_type_check(sme, 0, SM_CX_TYPE);
      if (cx->my_type != SM_CX_TYPE)
        return (((sm_object *)cx));
      sm_cx_clear(cx);
      return (((sm_object *)sms_true));
    }
    case SM_CX_IMPORT_EXPR: {
      sm_cx *cxFrom = (sm_cx *)eager_type_check(sme, 0, SM_CX_TYPE);
      if (cxFrom->my_type != SM_CX_TYPE)
        return (((sm_object *)cxFrom));
      sm_cx *cxTo = (sm_cx *)eager_type_check(sme, 1, SM_CX_TYPE);
      if (cxTo->my_type != SM_CX_TYPE)
        return (((sm_object *)cxTo));
      sm_cx_import(cxFrom, cxTo);
      return (((sm_object *)sms_true));
    }
    case SM_CX_CONTAINING_EXPR: {
      sm_cx *cx = (sm_cx *)eager_type_check(sme, 0, SM_CX_TYPE);
      if (cx->my_type != SM_CX_TYPE)
        return (((sm_object *)cx));
      sm_symbol *sym = (sm_symbol *)eager_type_check(sme, 1, SM_SYMBOL_TYPE);
      if (sym->my_type != SM_SYMBOL_TYPE)
        return (((sm_object *)sym));
      sm_cx *retrieved = sm_cx_get_container(cx, sym);
      if (retrieved)
        return (((sm_object *)retrieved));
      return (((sm_object *)sms_false));
    }
    case SM_CX_SIZE_EXPR: {
      sm_cx *cx = (sm_cx *)eager_type_check(sme, 0, SM_CX_TYPE);
      if (cx->my_type != SM_CX_TYPE)
        return (((sm_object *)cx));
      uint32_t size = sm_cx_size(cx);
      return (((sm_object *)sm_new_f64(size)));
    }
    case SM_RM_EXPR: {
      sm_symbol *sym = (sm_symbol *)eager_type_check(sme, 0, SM_SYMBOL_TYPE);
      // TODO: use cx stack here
      // bool success = sm_cx_rm(current_cx, sym);
      bool success = false;
      if (success == true)
        return (((sm_object *)sms_true));
      return (((sm_object *)sms_false));
    }
    case SM_CX_RM_EXPR: {
      sm_cx *cx = (sm_cx *)eager_type_check(sme, 0, SM_CX_TYPE);
      if (cx->my_type != SM_CX_TYPE)
        return (((sm_object *)cx));
      sm_symbol *sym = (sm_symbol *)eager_type_check(sme, 1, SM_SYMBOL_TYPE);
      if (sym->my_type != SM_SYMBOL_TYPE)
        return (((sm_object *)sym));
      bool success = sm_cx_rm(cx, sym);
      if (success == true)
        return (((sm_object *)sms_true));
      return (((sm_object *)sms_false));
    }
    case SM_CX_KEYS_EXPR: {
      sm_cx *cx = (sm_cx *)eager_type_check(sme, 0, SM_CX_TYPE);
      if (cx->my_type != SM_CX_TYPE)
        return (((sm_object *)cx));
      sm_expr *success =
        sm_node_keys(cx->content, sm_new_stack_obj(32), sm_new_expr_n(SM_TUPLE_EXPR, 0, 0, NULL));
      if (success)
        return (((sm_object *)success));
      return (((sm_object *)sms_false));
    }
    case SM_CX_VALUES_EXPR: {
      sm_cx *cx = (sm_cx *)sm_eval(sm_expr_get_arg(sme, 0));
      if (cx->my_type != SM_CX_TYPE) {
        sm_symbol *title   = sm_new_symbol("typeMismatch", 12);
        sm_string *message = sm_new_fstring_at(
          sms_heap, "Passed value of type %s to argument 0 of cxValues call. Expected Cx.",
          sm_type_name(cx->my_type));
        return (((sm_object *)sm_new_error_from_expr(title, message, sme, NULL)));
      }
      return (((sm_object *)sm_node_values(cx->content, sm_new_expr_n(SM_TUPLE_EXPR, 0, 0, NULL))));
    }
    case SM_FN_XP_EXPR: {
      sm_fun *fun = (sm_fun *)eager_type_check(sme, 0, SM_FUN_TYPE);
      if (fun->my_type != SM_FUN_TYPE)
        return (((sm_object *)fun));
      return ((sm_unlocalize((sm_object *)fun->content)));
    }
    case SM_FN_SETXP_EXPR: {
      sm_fun *fun = (sm_fun *)eager_type_check(sme, 0, SM_FUN_TYPE);
      if (fun->my_type != SM_FUN_TYPE)
        return (((sm_object *)fun));
      fun                  = (sm_fun *)sm_copy((sm_object *)fun); // functional
      sm_object *evaluated = sm_eval(sm_expr_get_arg(sme, 1));
      fun->content         = sm_localize(evaluated, fun);
      return (((sm_object *)fun));
    }
    case SM_FN_PARAMS_EXPR: {
      sm_fun *fun = (sm_fun *)eager_type_check(sme, 0, SM_FUN_TYPE);
      if (fun->my_type != SM_FUN_TYPE)
        return (((sm_object *)fun));
      sm_expr *result = sm_new_expr_n(SM_PARAM_LIST_EXPR, fun->num_params, fun->num_params, NULL);
      for (uint32_t i = 0; i < fun->num_params; i++) {
        sm_string *fn_name = sm_fun_get_param(fun, i)->name;
        sm_expr_set_arg(result, i, (sm_object *)sm_new_symbol(&(fn_name->content), fn_name->size));
      }
      return (((sm_object *)result));
    }
    case SM_FN_SETPARAMS_EXPR: {
      sm_fun *fun = (sm_fun *)eager_type_check(sme, 0, SM_FUN_TYPE);
      if (fun->my_type != SM_FUN_TYPE)
        return (((sm_object *)fun));
      sm_expr *params = (sm_expr *)eager_type_check(sme, 1, SM_EXPR_TYPE);
      if (params->my_type != SM_EXPR_TYPE)
        return (((sm_object *)params));
      // Make a new function with the right size (params are part of a function)
      sm_fun *new_fun = sm_new_fun(fun->parent, params->size, fun->content);
      // Checking the parameters
      for (uint32_t i = 0; i < params->size; i++) {
        sm_symbol *sym = (sm_symbol *)sm_expr_get_arg(params, i);
        if (sym->my_type != SM_SYMBOL_TYPE) {
          sm_symbol *title = sm_new_symbol("typeMismatch", 12);
          sm_string *message =
            sm_new_fstring_at(sms_heap,
                              "When using fnSetParams(<fn>,<params>), params must be a tuple of "
                              "symbols. Parameter %i is not a symbol.",
                              i);
          return (((sm_object *)sm_new_error_from_expr(title, message, sme, NULL)));
        }
      }
      // Setting the parameters of a new function
      for (uint32_t i = 0; i < params->size; i++) {
        sm_symbol *sym = (sm_symbol *)sm_expr_get_arg(params, i);
        sm_fun_set_param(new_fun, i, sym->name, NULL);
      }
      // Relocalizing the AST
      new_fun->content    = sm_unlocalize(new_fun->content);
      new_fun->num_params = params->size;
      new_fun->content    = sm_localize(new_fun->content, new_fun);
      return (((sm_object *)new_fun));
    } break;
      /*
      case SM_FN_PARENT_EXPR: {
      sm_eval(sm_expr_get_arg(sme, 0));
      sm_fun *fun = (sm_fun *)return_obj;
      if (!expect_type((sm_object *)fun, SM_FUN_TYPE))
      return(((sm_object *)sms_false));

      if (fun->parent) {
      return((sm_object *)fun->parent);
      } else {
      return((sm_object *)sms_false);
      }
      }
      case SM_FN_SETPARENT_EXPR: {
      sm_eval(sm_expr_get_arg(sme, 0));
      sm_fun *fun = (sm_fun *)return_obj;
      sm_eval(sm_expr_get_arg(sme, 1));
      sm_cx *new_parent = (sm_cx *)return_obj;
      if (!expect_type((sm_object *)fun, SM_FUN_TYPE))
      return((sm_object *)sms_false);
      if (!expect_type((sm_object *)new_parent, SM_CX_TYPE))
      return((sm_object *)sms_false);
      fun         = (sm_fun *)sm_copy((sm_object *)fun);
      fun->parent = new_parent;
      return((sm_object *)fun);
      }
      case SM_XP_OP_EXPR: {
      sm_eval(sm_expr_get_arg(sme, 0));
      sm_object *obj0 = return_obj;
      if (!expect_type(obj0, SM_EXPR_TYPE))
      return(((sm_object *)sms_false));
      sm_expr *expression = (sm_expr *)obj0;
      return(((sm_object *)sm_new_f64(expression->op)));
      }
      case SM_XP_SET_OP_EXPR: {
      sm_eval(sm_expr_get_arg(sme, 0));
      sm_object *obj0 = return_obj;
      if (!expect_type(obj0, SM_EXPR_TYPE))
      return(((sm_object *)sms_false));
      sm_eval(sm_expr_get_arg(sme, 1));
      sm_object *obj1 = return_obj;
      if (!expect_type(obj1, SM_F64_TYPE))
      return(((sm_object *)sms_false));
      sm_expr *expression = (sm_expr *)obj0;
      expression          = (sm_expr *)sm_copy((sm_object *)expression);
      sm_f64 *given_op    = (sm_f64 *)obj1;
      expression->op      = (uint32_t)((sm_f64 *)given_op)->value;
      return(((sm_object *)expression));
      }
      case SM_XP_OP_SYM_EXPR: {
      sm_eval(sm_expr_get_arg(sme, 0));
      sm_object *obj0 = return_obj;
      if (!expect_type(obj0, SM_EXPR_TYPE))
      return(((sm_object *)sms_false));
      uint32_t op_num = ((sm_expr *)obj0)->op;
      return(
      ((sm_object *)sm_new_symbol(sm_global_fn_name(op_num), sm_global_fn_name_len(op_num))));
      }
      case SM_STR_ESCAPE_EXPR: {
      sm_eval(sm_expr_get_arg(sme, 0));
      sm_object *obj0 = return_obj;
      sm_string *str0;
      if (!expect_type(obj0, SM_STRING_TYPE))
      return(((sm_object *)sms_false));
      str0 = (sm_string *)obj0;
      return(((sm_object *)sm_string_escape(str0)));
      }
      case SM_INPUT_EXPR: {
      if (sm_global_environment(NULL)->plain_mode) {
      char input_str[500];
      fgets(input_str, sizeof(input_str), stdin);
      // we remove the trailing newline character
      uint32_t len       = strlen(input_str);
      input_str[len - 1] = '\0';
      return(((sm_object *)sm_new_string(len - 1, input_str)));
      }
      char *line = linenoise("");
      return(((sm_object *)sm_new_string(strlen(line), line)));
      }
      case SM_ARGS_EXPR: {
      if (sf)
      return(((sm_object *)sf));
      return(((sm_object *)sms_false));
      }
      case SM_OR_EXPR: {
      sm_eval(sm_expr_get_arg(sme, 0));
      sm_object *obj0 = return_obj;
      if (!IS_FALSE(obj0)) {
      return((sm_object *)sms_true);
      }
      sm_eval(sm_expr_get_arg(sme, 1));
      sm_object *obj1 = return_obj;
      if (!IS_FALSE(obj1)) {
      return(((sm_object *)sms_true));
      }
      return(((sm_object *)sms_false));
      }
      case SM_AND_EXPR: {
      sm_eval(sm_expr_get_arg(sme, 0));
      sm_object *obj0 = return_obj;
      if (IS_FALSE(obj0))
      return(((sm_object *)sms_false));
      sm_eval(sm_expr_get_arg(sme, 1));
      sm_object *obj1 = return_obj;
      if (IS_FALSE(obj1))
      return(((sm_object *)sms_false));
      return(((sm_object *)sms_true));
      }
      case SM_XOR_EXPR: {
      sm_eval(sm_expr_get_arg(sme, 0));
      sm_object *obj0 = return_obj;
      sm_eval(sm_expr_get_arg(sme, 1));
      sm_object *obj1 = return_obj;

      // XOR is true if exactly one of obj0 or obj1 is true
      bool is_obj0_true = !IS_FALSE(obj0);
      bool is_obj1_true = !IS_FALSE(obj1);

      if (is_obj0_true != is_obj1_true) {
      return(((sm_object *)sms_true));
      } else {
      return(((sm_object *)sms_false));
      }
      }
      case SM_NOT_EXPR: {
      sm_eval(sm_expr_get_arg(sme, 0));
      sm_object *obj0 = return_obj;
      if (!IS_FALSE(obj0))
      return(((sm_object *)sms_false));
      return(((sm_object *)sms_true));
      }
      case SM_OREQ_EXPR: {
      sm_symbol *sym           = (sm_symbol *)sm_expr_get_arg(sme, 0);
      sm_object *current_value = sm_cx_get(current_cx, sym);
      sm_eval(sm_expr_get_arg(sme, 1));
      sm_object *value = return_obj;

      if (current_value != (sm_object *)sms_false || value != (sm_object *)sms_false) {
      if (!sm_cx_set(current_cx, sym, (sm_object *)sms_true)) {
      sm_symbol *title   = sm_new_symbol("contextUpdateFailed", 19);
      sm_string *message = sm_new_fstring_at(sms_heap, "Failed to update symbol in context.");
      return(((sm_object *)sm_new_error_from_expr(title, message, sme, NULL)));
      }
      return((sm_object *)sms_true);
      } else {
      return((sm_object *)sms_false);
      }
      }

      case SM_ANDEQ_EXPR: {
      sm_symbol *sym           = (sm_symbol *)sm_expr_get_arg(sme, 0);
      sm_object *current_value = sm_cx_get(current_cx, sym);
      sm_eval(sm_expr_get_arg(sme, 1));
      sm_object *value = return_obj;

      if (current_value == (sm_object *)sms_false || value == (sm_object *)sms_false) {
      if (!sm_cx_set(current_cx, sym, (sm_object *)sms_false)) {
      sm_symbol *title   = sm_new_symbol("contextUpdateFailed", 19);
      sm_string *message = sm_new_fstring_at(sms_heap, "Failed to update symbol in context.");
      return(((sm_object *)sm_new_error_from_expr(title, message, sme, NULL)));
      }
      return((sm_object *)sms_false);
      } else {
      return((sm_object *)sms_true);
      }
      }
      case SM_XOREQ_EXPR: {
      sm_symbol *sym           = (sm_symbol *)sm_expr_get_arg(sme, 0);
      sm_object *current_value = sm_cx_get(current_cx, sym);
      sm_eval(sm_expr_get_arg(sme, 1));
      sm_object *value = return_obj;

      bool is_current_true = (current_value != (sm_object *)sms_false);
      bool is_value_true   = (value != (sm_object *)sms_false);

      if (is_current_true != is_value_true) {
      if (!sm_cx_set(current_cx, sym, (sm_object *)sms_true)) {
      sm_symbol *title   = sm_new_symbol("contextUpdateFailed", 19);
      sm_string *message = sm_new_fstring_at(sms_heap, "Failed to update symbol in context.");
      return(((sm_object *)sm_new_error_from_expr(title, message, sme, NULL)));
      }
      return((sm_object *)sms_true);
      } else {
      return((sm_object *)sms_false);
      }
      }
      case SM_NOTEQ_EXPR: {
      // Perform eager type checking for both arguments
      sm_eval(sm_expr_get_arg(sme, 0));
      sm_object *obj0 = return_obj;
      sm_eval(sm_expr_get_arg(sme, 1));
      sm_object *obj1 = return_obj;
      // Check if the two objects are equal
      if (sm_object_eq(obj0, obj1)) {
      // They are equal, return sms_false
      return((sm_object *)sms_false);
      } else {
      // They are not equal, return sms_true
      return((sm_object *)sms_true);
      }
      }
      case SM_ROUND_EXPR: {
      sm_eval(sm_expr_get_arg(sme, 0));
      sm_object *obj0 = return_obj;
      sm_f64    *number;
      if (expect_type(obj0, SM_F64_TYPE))
      number = (sm_f64 *)obj0;
      else
      return(((sm_object *)sm_new_string(0, "")));
      f64      val       = number->value;
      uint32_t floor_val = val > 0 ? val + 0.5 : val - 0.5;
      return(((sm_object *)sm_new_f64(floor_val)));
      }
      case SM_FLOOR_EXPR: {
      sm_eval(sm_expr_get_arg(sme, 0));
      sm_object *obj0 = return_obj;
      sm_f64    *number;
      if (expect_type(obj0, SM_F64_TYPE))
      number = (sm_f64 *)obj0;
      else
      return(((sm_object *)sm_new_string(0, "")));
      f64 val = number->value;
      return(((sm_object *)sm_new_f64(floor(val))));
      }
      case SM_CEIL_EXPR: {
      sm_eval(sm_expr_get_arg(sme, 0));
      sm_object *obj0 = return_obj;
      sm_f64    *number;
      if (expect_type(obj0, SM_F64_TYPE))
      number = (sm_f64 *)obj0;
      else
      return(((sm_object *)sm_new_string(0, "")));
      f64 val = number->value;
      return(((sm_object *)sm_new_f64(ceil(val))));
      }
      case SM_MOD_EXPR: {
      sm_eval(sm_expr_get_arg(sme, 0));
      sm_object *obj0 = return_obj;
      sm_eval(sm_expr_get_arg(sme, 1));
      sm_object *obj1 = return_obj;
      if (!expect_type(obj0, SM_F64_TYPE))
      return(((sm_object *)sms_false));
      sm_f64 *num0 = (sm_f64 *)obj0;
      if (!expect_type(obj1, SM_F64_TYPE))
      return(((sm_object *)sms_false));
      sm_f64 *num1 = (sm_f64 *)obj1;
      return(((sm_object *)sm_new_f64(fmod(num0->value, num1->value))));
      }
      case SM_RANDOM_EXPR: {
      return(((sm_object *)sm_new_f64(((f64)rand()) / ((f64)RAND_MAX))));
      }
      case SM_SEED_EXPR: {
      eager_type_check(sme, 0, SM_F64_TYPE);
      sm_object *obj0 = return_obj;
      if (obj0->my_type != SM_F64_TYPE)
      return;
      sm_f64  *number    = (sm_f64 *)obj0;
      f64      val       = number->value;
      uint32_t floor_val = val > 0 ? val + 0.5 : val - 0.5;
      srand((int)floor_val);
      return_obj = (sm_object *)sms_true;
      return;
      }
      case SM_FILE_WRITESTR_EXPR: {
      // Obtain the file name using eager_type_check
      eager_type_check(sme, 0, SM_STRING_TYPE);
      sm_string *fname_str = (sm_string *)return_obj;
      if (fname_str->my_type == SM_ERR_TYPE)
      return(((sm_object *)fname_str)); // return(the error if type check fails);
      char *fname_cstr = &(fname_str->content);
      // Obtain the content to write using eager_type_check
      eager_type_check(sme, 1, SM_STRING_TYPE);
      sm_string *content_str = (sm_string *)return_obj;
      if (content_str->my_type == SM_ERR_TYPE)
      return(((sm_object *)content_str)); // return(the error if type check fails);
      char *content_cstr = &(content_str->content);
      // Open the file for writing
      FILE *fptr = fopen(fname_cstr, "wb");
      if (fptr == NULL) {
      char error_msg[256];
      snprintf(error_msg, sizeof(error_msg), "fileWrite failed to open: %s", fname_cstr);
      sm_symbol *title   = sm_new_symbol("fileOpenError", strlen("fileOpenError"));
      sm_string *message = sm_new_string(strlen(error_msg), error_msg);
      return(((sm_object *)sm_new_error_from_expr(title, message, sme, NULL)));
      }
      // Write content to file in chunks of 1024 bytes
      size_t       total_written = 0;
      const size_t CHUNK_SIZE    = 1024;
      while (total_written < content_str->size) {
      size_t to_write = CHUNK_SIZE;
      if (total_written + CHUNK_SIZE > content_str->size) {
      to_write       = content_str->size - total_written;
      size_t written = fwrite(content_cstr + total_written, 1, to_write, fptr);
      if (written != to_write) {
      fclose(fptr);
      sm_symbol *title   = sm_new_symbol("fileWriteError", strlen("fileWriteError"));
      sm_string *message = sm_new_string(strlen("fileWrite failed during write operation"),
                                  "fileWrite failed during write operation");
      return(((sm_object *)sm_new_error_from_expr(title, message, sme, NULL)));
      }
      total_written += written;
      }
      // Close the file
      fclose(fptr);
      return(((sm_object *)sms_true));
      }
      }
      case SM_FILE_WRITEARR_EXPR: {
      // Obtain the file name using eager_type_check
      eager_type_check(sme, 0, SM_STRING_TYPE);
      sm_string *fname_str = (sm_string *)return_obj;
      if (fname_str->my_type == SM_ERR_TYPE)
      return(((sm_object *)fname_str)); // return(the error if type check fails);
      char *fname_cstr = &(fname_str->content);
      // Obtain the sm_array content to write using eager_type_check
      eager_type_check(sme, 1, SM_ARRAY_TYPE);
      sm_array *content_array = (sm_array *)return_obj;
      if (content_array->my_type == SM_ERR_TYPE)
      return(((sm_object *)content_array)); // return(the error if type check fails);
      // Determine the content type and set up the appropriate pointer and size
      char  *content_cstr = NULL;
      size_t content_size = 0;
      switch (content_array->inner_type) {
      case SM_UI8_TYPE: {
      // Handle ui8 arrays
      content_cstr = (char *)content_array->content + sizeof(sm_space);
      content_size = content_array->size; // Size in bytes
      break;
      }
      case SM_F64_TYPE: {
      // Handle f64 arrays
      content_cstr = (char *)content_array->content + sizeof(sm_space);
      content_size = content_array->size * sizeof(f64); // Size in bytes
      break;
      }
      default: {
      // Unknown type; return an error
      sm_symbol *title   = sm_new_symbol("unsupportedTypeError", strlen("unsupportedTypeError"));
      sm_string *message = sm_new_string(strlen("Unsupported array type for writing"),
                              "Unsupported array type for writing");
      return(((sm_object *)sm_new_error_from_expr(title, message, sme, NULL)));
      }
      }
      // Open the file for writing
      FILE *fptr = fopen(fname_cstr, "wb");
      if (fptr == NULL) {
      char error_msg[256];
      snprintf(error_msg, sizeof(error_msg), "fileWrite failed to open: %s", fname_cstr);
      sm_symbol *title   = sm_new_symbol("fileOpenError", strlen("fileOpenError"));
      sm_string *message = sm_new_string(strlen(error_msg), error_msg);
      return(((sm_object *)sm_new_error_from_expr(title, message, sme, NULL)));
      }
      // Write content to file in chunks of 1024 bytes
      size_t       total_written = 0;
      const size_t CHUNK_SIZE    = 1024;
      while (total_written < content_size) {
      size_t to_write = CHUNK_SIZE;
      if (total_written + CHUNK_SIZE > content_size)
      to_write = content_size - total_written;
      size_t written = fwrite(content_cstr + total_written, 1, to_write, fptr);
      if (written != to_write) {
      fclose(fptr);
      sm_symbol *title   = sm_new_symbol("fileWriteError", strlen("fileWriteError"));
      sm_string *message = sm_new_string(strlen("fileWrite failed during write operation"),
                                "fileWrite failed during write operation");
      return(((sm_object *)sm_new_error_from_expr(title, message, sme, NULL)));
      }
      total_written += written;
      }
      // Close the file
      fclose(fptr);
      return(((sm_object *)sms_true));
      }
      case SM_FILE_APPEND_EXPR: {
      // obtain the file name
      sm_string *fname_str;
      sm_eval(sm_expr_get_arg(sme, 0));
      sm_object *evaluated_fname = return_obj;
      if (!expect_type(evaluated_fname, SM_STRING_TYPE))
      return(((sm_object *)sms_false));
      fname_str        = (sm_string *)evaluated_fname;
      char *fname_cstr = &(fname_str->content);
      // obtain the content to write
      sm_eval(sm_expr_get_arg(sme, 1));
      sm_object *evaluated_content = return_obj;
      sm_string *content_str;
      if (!expect_type(evaluated_content, SM_STRING_TYPE))
      return(((sm_object *)sms_false));
      content_str        = (sm_string *)evaluated_content;
      char *content_cstr = &(content_str->content);
      FILE *fptr         = fopen(fname_cstr, "a");
      // check that file can be opened for writing
      if (fptr == NULL) {
      printf("fileAppendStr failed to open for appending: %s\n", fname_cstr);
      return(((sm_object *)sms_false));
      }
      fputs(content_cstr, fptr);
      fclose(fptr);
      return(((sm_object *)sms_true));
      }
      case SM_FILE_READSTR_EXPR: {
      // Evaluate and check if the first argument is a string (file name)
      eager_type_check(sme, 0, SM_STRING_TYPE);
      sm_string *fname_str = (sm_string *)return_obj;
      if (fname_str->my_type == SM_ERR_TYPE)
      return(((sm_object *)fname_str)); // return(the error if type check fails);
      char *fname_cstr = &(fname_str->content);
      // Check if the file exists
      if (access(fname_cstr, F_OK) != 0) {
      char error_msg[256];
      snprintf(error_msg, sizeof(error_msg),
      "fileReadStr failed because the file, %s, does not exist.", fname_cstr);
      sm_symbol *title   = sm_new_symbol("fileNotFoundError", strlen("fileNotFoundError"));
      sm_string *message = sm_new_string(strlen(error_msg), error_msg);
      return(((sm_object *)sm_new_error_from_expr(title, message, sme, NULL)));
      }
      // Open the file for reading
      FILE *fptr = fopen(fname_cstr, "r");
      if (fptr == NULL) {
      char error_msg[256];
      snprintf(error_msg, sizeof(error_msg), "fileReadStr failed to open: %s", fname_cstr);
      sm_symbol *title   = sm_new_symbol("fileOpenError", strlen("fileOpenError"));
      sm_string *message = sm_new_string(strlen(error_msg), error_msg);
      return(((sm_object *)sm_new_error_from_expr(title, message, sme, NULL)));
      }
      // Determine the file length
      fseek(fptr, 0, SEEK_END);
      long len = ftell(fptr);
      if (len < 0) {
      fclose(fptr);
      sm_symbol *title   = sm_new_symbol("fileReadStrError", strlen("fileReadStrError"));
      sm_string *message = sm_new_string(strlen("fileReadStr failed to determine file length"),
                              "fileReadStr failed to determine file length");
      return(((sm_object *)sm_new_error_from_expr(title, message, sme, NULL)));
      }
      // Read the file contents
      sm_string *output = sm_new_string_manual(len);
      fseek(fptr, 0, SEEK_SET);
      size_t read_count = fread(&(output->content), 1, len, fptr);
      if (read_count != len) {
      fclose(fptr);
      sm_symbol *title   = sm_new_symbol("fileReadStrError", strlen("fileReadStrError"));
      sm_string *message = sm_new_string(strlen("fileReadStr failed during read operation"),
                              "fileReadStr failed during read operation");
      return(((sm_object *)sm_new_error_from_expr(title, message, sme, NULL)));
      }
      fclose(fptr);
      // Null-terminate the output string
      (&output->content)[len] = '\0';
      return(((sm_object *)output));
      }
      case SM_FILE_READ_EXPR: {
      // Evaluate and check if the first argument is a string (file name)
      eager_type_check(sme, 0, SM_STRING_TYPE);
      sm_string *fname_str = (sm_string *)return_obj;
      if (fname_str->my_type == SM_ERR_TYPE)
      return((sm_object *)fname_str); // Return the error if type check fails

      char *fname_cstr = &(fname_str->content);

      // Check if the file exists
      if (access(fname_cstr, F_OK) != 0) {
      char error_msg[256];
      snprintf(error_msg, sizeof(error_msg),
      "fileRead failed because the file, %s, does not exist.", fname_cstr);
      sm_symbol *title   = sm_new_symbol("fileNotFoundError", strlen("fileNotFoundError"));
      sm_string *message = sm_new_string(strlen(error_msg), error_msg);
      return((sm_object *)sm_new_error_from_expr(title, message, sme, NULL));
      }

      // Open the file for reading in binary mode
      FILE *fptr = fopen(fname_cstr, "rb");
      if (fptr == NULL) {
      char error_msg[256];
      snprintf(error_msg, sizeof(error_msg), "fileRead failed to open: %s", fname_cstr);
      sm_symbol *title   = sm_new_symbol("fileOpenError", strlen("fileOpenError"));
      sm_string *message = sm_new_string(strlen(error_msg), error_msg);
      return((sm_object *)sm_new_error_from_expr(title, message, sme, NULL));
      }

      // Determine the file length
      fseek(fptr, 0, SEEK_END);
      long len = ftell(fptr);
      if (len < 0) {
      fclose(fptr);
      sm_symbol *title   = sm_new_symbol("fileReadError", strlen("fileReadError"));
      sm_string *message = sm_new_string(strlen("fileRead failed to determine file length"),
                              "fileRead failed to determine file length");
      return((sm_object *)sm_new_error_from_expr(title, message, sme, NULL));
      }

      // Create a new ui8 array to store the file contents
      sm_space *file_content_space = sm_new_space(len);
      uint8_t  *file_content       = (uint8_t *)(&file_content_space[1]);

      // Read the file contents
      fseek(fptr, 0, SEEK_SET);
      size_t read_count = fread(file_content, 1, len, fptr);
      if (read_count != len) {
      fclose(fptr);
      sm_symbol *title   = sm_new_symbol("fileReadError", strlen("fileReadError"));
      sm_string *message = sm_new_string(strlen("fileRead failed during read operation"),
                              "fileRead failed during read operation");
      return((sm_object *)sm_new_error_from_expr(title, message, sme, NULL));
      }

      fclose(fptr);

      // Create a new ui8 array object
      sm_array *ui8_array =
      sm_new_array(SM_UI8_TYPE, len, (sm_object *)file_content_space, sizeof(sm_space));
      return((sm_object *)ui8_array);
      }

      case SM_FILE_PART_EXPR: {
      sm_eval(sm_expr_get_arg(sme, 0));
      sm_object *evaluated = return_obj;
      sm_string *fname_str;
      if (!expect_type(evaluated, SM_STRING_TYPE))
      return(((sm_object *)sms_false));
      fname_str        = (sm_string *)evaluated;
      char *fname_cstr = &(fname_str->content);
      sm_eval(sm_expr_get_arg(sme, 1));
      evaluated = return_obj;
      sm_f64 *start_pos;
      if (!expect_type(evaluated, SM_F64_TYPE))
      return(((sm_object *)sms_false));
      start_pos = (sm_f64 *)evaluated;
      sm_eval(sm_expr_get_arg(sme, 2));
      evaluated = return_obj;
      sm_f64 *length;
      if (!expect_type(evaluated, SM_F64_TYPE))
      return(((sm_object *)sms_false));
      length = (sm_f64 *)evaluated;
      if (access(fname_cstr, F_OK) != 0) {
      printf("filePart failed because the file, %s ,does not exist.\n", fname_cstr);
      return(((sm_object *)sm_new_string(0, "")));
      }
      FILE *fptr = fopen(fname_cstr, "r");
      fseek(fptr, start_pos->value, SEEK_SET);
      sm_string *output = sm_new_string_manual((uint32_t)length->value);
      fread(&(output->content), 1, (uint32_t)length->value, fptr);
      fclose(fptr);
      *(&output->content + ((uint32_t)length->value)) = '\0';
      return(((sm_object *)output));
      }
      case SM_FILE_EXISTS_EXPR: {
      sm_eval(sm_expr_get_arg(sme, 0));
      sm_object *evaluated = return_obj;
      sm_string *fname_str;
      if (!expect_type(evaluated, SM_STRING_TYPE))
      return(((sm_object *)sms_false));
      fname_str        = (sm_string *)evaluated;
      char *fname_cstr = &(fname_str->content);

      FILE *file = fopen(fname_cstr, "r");
      if (file == NULL) {
      return(((sm_object *)sms_false));
      }
      fclose(file);
      return(((sm_object *)sms_true));
      }
      case SM_FILE_RM_EXPR: {
      sm_eval(sm_expr_get_arg(sme, 0));
      sm_object *evaluated = return_obj;
      sm_string *fname_str;
      if (!expect_type(evaluated, SM_STRING_TYPE))
      return(((sm_object *)sms_false));
      fname_str           = (sm_string *)evaluated;
      char    *fname_cstr = &(fname_str->content);
      uint32_t result     = remove(fname_cstr);
      if (result != 0) {
      printf("fileRm failed: Could not rm file: %s\n", fname_cstr);
      return(((sm_object *)sms_false));
      }
      return(((sm_object *)sms_true));
      }
      case SM_FILE_STAT_EXPR: {
      sm_eval(sm_expr_get_arg(sme, 0));
      sm_object *evaluated = return_obj;
      sm_string *fname_str;
      if (!expect_type(evaluated, SM_STRING_TYPE))
      return(((sm_object *)sms_false));
      fname_str              = (sm_string *)evaluated;
      char       *fname_cstr = &(fname_str->content);
      struct stat filestat;
      sm_expr    *output;
      if (stat(fname_cstr, &filestat) == 0) {
      // build a tuple with stat information.
      output = sm_new_expr_n(SM_TUPLE_EXPR, 16, 16, NULL);
      sm_expr_set_arg(output, 0, (sm_object *)sm_new_f64(filestat.st_dev));
      sm_expr_set_arg(output, 1, (sm_object *)sm_new_f64(filestat.st_ino));
      sm_expr_set_arg(output, 2, (sm_object *)sm_new_f64(filestat.st_mode));
      sm_expr_set_arg(output, 3, (sm_object *)sm_new_f64(filestat.st_nlink));
      sm_expr_set_arg(output, 4, (sm_object *)sm_new_f64(filestat.st_uid));
      sm_expr_set_arg(output, 5, (sm_object *)sm_new_f64(filestat.st_gid));
      sm_expr_set_arg(output, 6, (sm_object *)sm_new_f64(filestat.st_rdev));
      sm_expr_set_arg(output, 7, (sm_object *)sm_new_f64(filestat.st_size));
      sm_expr_set_arg(output, 8, (sm_object *)sm_new_f64(filestat.st_blksize));
      sm_expr_set_arg(output, 9, (sm_object *)sm_new_f64(filestat.st_blocks));
      sm_expr_set_arg(output, 12, (sm_object *)sm_new_f64(filestat.st_mtime));
      sm_expr_set_arg(output, 14, (sm_object *)sm_new_f64(filestat.st_ctime));
      #if __APPLE__
      sm_expr_set_arg(output, 10, (sm_object *)sm_new_f64(filestat.st_atime));
      sm_expr_set_arg(output, 11, (sm_object *)sm_new_f64(0));
      sm_expr_set_arg(output, 13, (sm_object *)sm_new_f64(0));
      sm_expr_set_arg(output, 15, (sm_object *)sm_new_f64(0));
      #else
      sm_expr_set_arg(output, 10, (sm_object *)sm_new_f64(filestat.st_atim.tv_sec));
      sm_expr_set_arg(output, 11, (sm_object *)sm_new_f64(filestat.st_atim.tv_nsec));
      sm_expr_set_arg(output, 13, (sm_object *)sm_new_f64(filestat.st_mtim.tv_nsec));
      sm_expr_set_arg(output, 15, (sm_object *)sm_new_f64(filestat.st_ctim.tv_nsec));
      #endif
      } else {
      printf("Failed to get file information.\n");
      return(((sm_object *)sms_false));
      }
      return(((sm_object *)output));
      }
      case SM_FILE_PARSE_EXPR: {
      sm_eval(sm_expr_get_arg(sme, 0));
      sm_object *evaluated = return_obj;
      sm_string *str;
      if (!expect_type(evaluated, SM_STRING_TYPE))
      return(((sm_object *)sms_false));
      str                  = (sm_string *)evaluated;
      char           *cstr = &(str->content);
      sm_parse_result pr   = sm_parse_file(cstr);
      if (pr.return_val != 0) {
      printf("Error: Parser failed and returned %i.\n", pr.return_val);
      return(((sm_object *)sms_false));
      }
      return((pr.parsed_object));
      break;
      }
      case SM_PARSE_EXPR: {
      sm_eval(sm_expr_get_arg(sme, 0));
      sm_object *evaluated = return_obj;
      sm_string *str;
      if (!expect_type(evaluated, SM_STRING_TYPE))
      return(((sm_object *)sms_false));
      str                = (sm_string *)evaluated;
      char *cstr         = &(str->content);
      cstr[str->size]    = ';'; // Temporarily replacing the NULL char
      sm_parse_result pr = sm_parse_cstr(cstr, str->size + 1);
      cstr[str->size]    = '\0'; // Place the null char back
      if (pr.return_val != 0) {
      printf("Error: Parser failed and returned %i.\n", pr.return_val);
      return(((sm_object *)sms_false));
      }
      return((pr.parsed_object));
      }
      case SM_NEW_STR_EXPR: {
      sm_eval(sm_expr_get_arg(sme, 0));
      sm_object *evaluated = return_obj;
      return(((sm_object *)sm_object_to_string(evaluated)));
      }
      case SM_EVAL_EXPR: {
      sm_eval(sm_expr_get_arg(sme, 0));
      sm_object *evaluated = return_obj;
      sm_eval(evaluated);
      return;
      }
      case SM_CX_EVAL_EXPR: {
      sm_eval(sm_expr_get_arg(sme, 1));
      sm_object *obj1 = return_obj;
      eager_type_check(sme, 0, SM_CX_TYPE);
      sm_object *evaluated = return_obj;
      sm_eval(obj1, (sm_cx *)evaluated, sf);
      return;
      }
      case SM_PUT_EXPR: {
      sm_string *str;
      for (int i = 0; i < sme->size; i++) {
      eager_type_check(sme, i, SM_STRING_TYPE);
      sm_object *evaluated = return_obj;
      if (evaluated->my_type != SM_STRING_TYPE)
      return(evaluated);
      str = (sm_string *)evaluated;
      for (uint32_t i = 0; i < str->size; i++)
      putchar((&str->content)[i]);
      }
      fflush(stdout);
      return(((sm_object *)sms_true));
      break;
      }
      case SM_PUTLN_EXPR: {
      sm_string *str;
      eager_type_check(sme, 0, SM_STRING_TYPE);
      sm_object *evaluated = return_obj;
      if (evaluated->my_type == SM_ERR_TYPE)
      return(evaluated);
      str = (sm_string *)evaluated;
      for (uint32_t i = 0; i < str->size; i++)
      putchar((&str->content)[i]);
      putchar('\n');
      fflush(stdout);
      return(((sm_object *)sms_true));
      break;
      }
      case SM_WHILE_EXPR: {
      sm_expr   *condition  = (sm_expr *)sm_expr_get_arg(sme, 0);
      sm_object *expression = sm_expr_get_arg(sme, 1);
      sm_eval((sm_object *)condition);
      sm_object *condition_result = return_obj;
      sm_object *eval_result;
      while (!IS_FALSE(condition_result)) {
      sm_eval(expression);
      eval_result = return_obj;
      sm_eval((sm_object *)condition);
      condition_result = return_obj;
      if (eval_result->my_type == SM_RETURN_TYPE)
      return(eval_result);
      }
      return(eval_result);
      break;
      }
      case SM_FOR_EXPR: {
      sm_expr   *init       = (sm_expr *)sm_expr_get_arg(sme, 0);
      sm_expr   *condition  = (sm_expr *)sm_expr_get_arg(sme, 1);
      sm_expr   *increment  = (sm_expr *)sm_expr_get_arg(sme, 2);
      sm_object *expression = sm_expr_get_arg(sme, 3);
      // Run init 1 time
      sm_eval((sm_object *)init);
      sm_object *result = (sm_object *)sms_false;
      sm_eval((sm_object *)condition);
      sm_object *condition_result = return_obj;
      sm_object *eval_result;
      while (!IS_FALSE(condition_result)) {
      sm_eval(expression);
      eval_result = return_obj;
      if (return_obj->my_type == SM_RETURN_TYPE || return_obj->my_type == SM_ERR_TYPE)
      return;
      // Run increment after each loop
      sm_eval((sm_object *)increment);
      sm_eval((sm_object *)condition);
      condition_result = return_obj;
      }
      return(eval_result);
      break;
      }
      case SM_FOR_IN_EXPR: {
      sm_symbol *handle             = (sm_symbol *)sm_expr_get_arg(sme, 0);
      sm_object *possibleCollection = sm_expr_get_arg(sme, 1);
      sm_expr   *expression         = (sm_expr *)sm_expr_get_arg(sme, 2);
      sm_object *result             = (sm_object *)sms_false;
      sm_eval(possibleCollection);
      sm_object *evalResult = return_obj;
      switch (evalResult->my_type) {
      case SM_ARRAY_TYPE: {
      sm_cx     *inner_cx = sm_new_cx(current_cx);
      sm_object *result   = (sm_object *)sms_false;
      sm_array  *arr      = (sm_array *)evalResult;
      sm_cx_let(inner_cx, handle, (sm_object *)sms_false);
      for (uint32_t i = 0; i < arr->size; i++) {
      sm_cx_set(inner_cx, handle, sm_array_get(arr, i));
      sm_eval((sm_object *)expression, inner_cx, sf);
      }
      return;
      break;
      }
      case SM_EXPR_TYPE: {
      sm_cx   *inner_cx       = sm_new_cx(current_cx);
      sm_expr *collectionExpr = (sm_expr *)evalResult;
      sm_cx_let(inner_cx, handle, (sm_object *)sms_false);
      for (uint32_t i = 0; i < collectionExpr->size; i++) {
      sm_cx_set(inner_cx, handle, sm_expr_get_arg(collectionExpr, i));
      sm_eval((sm_object *)expression, inner_cx, sf);
      }
      return;
      break;
      }
      case SM_STRING_TYPE: {
      sm_cx     *inner_cx = sm_new_cx(current_cx);
      sm_string *str      = (sm_string *)evalResult;
      sm_cx_let(inner_cx, handle, (sm_object *)sms_false);
      for (uint32_t i = 0; i < str->size; i++) {
      sm_cx_set(inner_cx, handle,
       (sm_object *)sm_new_fstring_at(sms_heap, "%c", (&str->content)[i]));
      sm_eval((sm_object *)expression, inner_cx, sf);
      }
      return;
      break;
      }
      default: {
      sm_string *message = sm_new_fstring_at(sms_heap,
                                  "Applying for-in to object that is not a collection "
                                  "(expr, array, string). Instead, object type is: %s",
                                  sm_type_name(evalResult->my_type));
      sm_symbol *title   = sm_new_symbol("typeMismatchForIn", 17);
      return((sm_object *)sm_new_error_from_expr(title, message, sme, NULL));
      }
      }
      break;
      }
      case SM_FOR_IN_WHERE_EXPR: {
      sm_symbol *handle             = (sm_symbol *)sm_expr_get_arg(sme, 0);
      sm_object *possibleCollection = sm_expr_get_arg(sme, 1);
      sm_expr   *expression         = (sm_expr *)sm_expr_get_arg(sme, 3);
      sm_expr   *whereCondition     = (sm_expr *)sm_expr_get_arg(sme, 2);
      sm_object *result             = (sm_object *)sms_false;
      sm_eval(possibleCollection);
      sm_object *evalResult = return_obj;
      switch (evalResult->my_type) {
      case SM_ARRAY_TYPE: {
      sm_cx     *inner_cx = sm_new_cx(current_cx);
      sm_object *result   = (sm_object *)sms_false;
      sm_array  *arr      = (sm_array *)evalResult;
      sm_cx_let(inner_cx, handle, (sm_object *)sms_false);
      for (uint32_t i = 0; i < arr->size; i++) {
      sm_cx_set(inner_cx, handle, sm_array_get(arr, i));
      sm_eval((sm_object *)whereCondition, inner_cx, sf);
      sm_object *conditionResult = return_obj;
      if (!IS_FALSE(conditionResult))
      sm_eval((sm_object *)expression, inner_cx, sf);
      }
      return;
      break;
      }
      case SM_EXPR_TYPE: {
      sm_cx   *inner_cx       = sm_new_cx(current_cx);
      sm_expr *collectionExpr = (sm_expr *)evalResult;
      sm_cx_let(inner_cx, handle, (sm_object *)sms_false);
      for (uint32_t i = 0; i < collectionExpr->size; i++) {
      sm_cx_set(inner_cx, handle, sm_expr_get_arg(collectionExpr, i));
      sm_eval((sm_object *)whereCondition, inner_cx, sf);
      sm_object *conditionResult = return_obj;
      if (!IS_FALSE(conditionResult))
      sm_eval((sm_object *)expression, inner_cx, sf);
      }
      return;
      break;
      }
      case SM_STRING_TYPE: {
      sm_cx     *inner_cx = sm_new_cx(current_cx);
      sm_string *str      = (sm_string *)evalResult;
      sm_cx_let(inner_cx, handle, (sm_object *)sms_false);
      for (uint32_t i = 0; i < str->size; i++) {
      sm_cx_set(inner_cx, handle,
       (sm_object *)sm_new_fstring_at(sms_heap, "%c", (&str->content)[i]));
      sm_eval((sm_object *)whereCondition, inner_cx, sf);
      sm_object *conditionResult = return_obj;
      if (!IS_FALSE(conditionResult))
      sm_eval((sm_object *)expression, inner_cx, sf);
      }
      return;
      break;
      }
      default: {
      sm_string *message = sm_new_fstring_at(sms_heap,
                                  "Applying for-in to object that is not a collection "
                                  "(expr, array, string). Instead, object type is: %s",
                                  sm_type_name(evalResult->my_type));
      sm_symbol *title   = sm_new_symbol("typeMismatchForIn", 17);
      return((sm_object *)sm_new_error_from_expr(title, message, sme, NULL));
      }
      }
      break;
      }
      case SM_DO_WHILE_EXPR: {
      sm_expr   *condition  = (sm_expr *)sm_expr_get_arg(sme, 1);
      sm_object *expression = sm_expr_get_arg(sme, 0);
      sm_object *result     = (sm_object *)sms_true;
      do {
      sm_eval(expression);
      result = return_obj;
      if (result->my_type == SM_RETURN_TYPE)
      return((result));
      sm_eval((sm_object *)condition);
      } while (!IS_FALSE(return_obj));
      return;
      break;
      }
      case SM_RETURN_EXPR: {
      sm_eval(sm_expr_get_arg(sme, 0));
      return(((sm_object *)sm_new_return(return_obj)));
      }
      case SM_SIZE_EXPR: {
      eager_type_check2(sme, 0, SM_EXPR_TYPE, SM_ARRAY_TYPE);
      sm_object *base_obj = return_obj;
      if (base_obj->my_type == SM_ERR_TYPE)
      return((base_obj));
      switch (base_obj->my_type) {
      case SM_EXPR_TYPE: {
      sm_expr *expr = (sm_expr *)base_obj;
      return(((sm_object *)sm_new_f64(expr->size)));
      }
      case SM_ARRAY_TYPE: {
      sm_array *array = (sm_array *)base_obj;
      return(((sm_object *)sm_new_f64(array->size)));
      }
      default:
      return(((sm_object *)sm_new_f64(0)));
      }
      }
      case SM_MAP_EXPR: {
      // expecting a unary func
      eager_type_check(sme, 0, SM_FUN_TYPE);
      if (return_obj->my_type != SM_FUN_TYPE) {
      return; // Return the function object if type check fails
      }
      sm_fun *fun = (sm_fun *)return_obj;

      eager_type_check(sme, 1, SM_EXPR_TYPE);
      if (return_obj->my_type != SM_EXPR_TYPE) {
      return;
      }
      sm_expr *arr = (sm_expr *)return_obj;
      // Create a new output expression with the same operation type and size
      sm_expr *output = sm_new_expr_n(arr->op, arr->size, arr->size, NULL);
      // Iterate over the elements of the array expression (arr)
      for (uint32_t i = 0; i < arr->size; i++) {
      sm_object *current_obj = sm_expr_get_arg(arr, i);
      sm_expr   *new_sf      = sm_new_expr(SM_PARAM_LIST_EXPR, current_obj, NULL);
      // Call the function with the new expression
      sm_eval(fun->content, fun->parent, new_sf);
      sm_object *map_result = return_obj;
      // If the result is a return type, dereference it
      if (map_result->my_type == SM_RETURN_TYPE) {
      map_result = ((sm_return *)map_result)->address;
      }
      // Set the result of the current element into the output expression
      sm_expr_set_arg(output, i, map_result);
      }
      // Return the output expression with the mapped results
      return((sm_object *)output);
      }
      case SM_REDUCE_EXPR: {
      // expecting a binary function
      sm_eval(sm_expr_get_arg(sme, 0));
      sm_object *obj0 = return_obj;
      sm_fun    *fun;
      if (!expect_type(obj0, SM_FUN_TYPE))
      return(((sm_object *)sms_false));
      fun = (sm_fun *)obj0;
      // evaluating the expression to reduce
      sm_eval(sm_expr_get_arg(sme, 1));
      sm_object *obj1 = return_obj;
      sm_expr   *arr;
      if (!expect_type(obj1, SM_EXPR_TYPE))
      return(((sm_object *)sms_false));
      arr = (sm_expr *)obj1;
      // initial value for reduction
      sm_eval(sm_expr_get_arg(sme, 2));
      sm_object *initial = return_obj;
      // reducing the expression
      sm_object *result = initial;
      // Evaluating with a reusable stack frame: ( result , current_ob )
      sm_expr *reusable = sm_new_expr_2(SM_PARAM_LIST_EXPR, result, NULL, NULL);
      for (uint32_t i = 0; i < arr->size; i++) {
      sm_object *current_obj = sm_expr_get_arg(arr, i);
      sm_expr_set_arg(reusable, 1, current_obj);
      sm_eval(fun->content, fun->parent, reusable);
      result = return_obj;
      sm_expr_set_arg(reusable, 0, result);
      }
      return((result));
      }
      case SM_INDEX_EXPR: {
      eager_type_check(sme, 1, SM_F64_TYPE);
      sm_f64 *index_f64 = (sm_f64 *)return_obj;
      if (index_f64->my_type != SM_F64_TYPE)
      return(((sm_object *)index_f64));
      uint32_t index = (uint32_t)index_f64->value;
      // obj could be sm_expr or sm_array
      eager_type_check2(sme, 0, SM_EXPR_TYPE, SM_ARRAY_TYPE);
      sm_expr *obj = (sm_expr *)return_obj;

      switch (obj->my_type) {
      case SM_EXPR_TYPE: {
      sm_expr *arr = (sm_expr *)obj;
      if (arr->size < index + 1 || index_f64->value < 0) {
      sm_string *message = sm_new_fstring_at(
      sms_heap, "Index out of range: %i . Expr size is %i", index, arr->size);
      sm_symbol *title = sm_new_symbol("indexOutOfBounds", 16);
      return(((sm_object *)sm_new_error_from_expr(title, message, sme, NULL)));
      }
      return((sm_expr_get_arg(arr, index)));
      }
      case SM_ARRAY_TYPE: {
      sm_array *arr = (sm_array *)obj;
      if (arr->size < index + 1 || index_f64->value < 0) {
      sm_string *message = sm_new_fstring_at(
      sms_heap, "Index out of range: %i . Array size is %i", index, arr->size);
      sm_symbol *title = sm_new_symbol("indexOutOfBounds", 16);
      return(((sm_object *)sm_new_error_from_expr(title, message, sme, NULL)));
      }
      return((sm_array_get(arr, index)));
      }
      default:
      return(((sm_object *)obj));
      }
      }
      case SM_DOT_EXPR: {
      eager_type_check(sme, 0, SM_CX_TYPE);
      sm_cx *base_cx = (sm_cx *)return_obj;
      if (base_cx->my_type == SM_ERR_TYPE)
      return(((sm_object *)base_cx));
      sm_symbol *field_sym  = (sm_symbol *)sm_expr_get_arg(sme, 1);
      sm_string *field_name = field_sym->name;
      sm_object *sr         = sm_cx_get_far(base_cx, field_sym);
      if (sr == NULL) {
      sm_symbol *title   = sm_new_symbol("varNotFound", 11);
      sm_string *cx_str  = sm_object_to_string((sm_object *)current_cx);
      sm_string *message = sm_new_fstring_at(sms_heap, "variable: %s not found in cx: %s",
                                  &field_name->content, &cx_str->content);
      return(((sm_object *)sm_new_error_from_expr(title, message, sme, NULL)));
      }
      return(((sm_object *)sr));
      }
      case SM_PARENT_EXPR: {
      eager_type_check(sme, 0, SM_CX_TYPE);
      sm_cx *base_cx = (sm_cx *)return_obj;
      if (base_cx->parent == NULL)
      return(((sm_object *)sms_false));
      return(((sm_object *)base_cx->parent));
      }
      case SM_DIFF_EXPR: {
      sm_eval(sm_expr_get_arg(sme, 0));
      sm_object *evaluated0 = return_obj;
      sm_eval(sm_expr_get_arg(sme, 1));
      sm_object *evaluated1 = return_obj;
      sm_symbol *sym1;
      if (!expect_type(evaluated1, SM_SYMBOL_TYPE))
      return(((sm_object *)sms_false));
      sym1              = (sm_symbol *)evaluated1;
      sm_object *result = sm_diff(evaluated0, sym1);
      return((sm_simplify(result)));
      }
      case SM_SIMP_EXPR: {
      sm_eval(sm_expr_get_arg(sme, 0));
      sm_object *evaluated0 = return_obj;
      return((sm_simplify(evaluated0)));
      }
      case SM_FUN_CALL_EXPR: {
      return_obj = (sm_object *)sm_expr_get_arg(sme, 1);
      sm_eval(return_obj);
      sm_expr   *new_args         = (sm_expr *)return_obj;
      sm_object *function_to_call = sm_expr_get_arg(sme, 0);
      sm_eval(function_to_call);
      if (return_obj->my_type == SM_ERR_TYPE)
      return; // return the return_obj
      execute_fun((sm_fun *)return_obj, current_cx, new_args);
      return;
      break;
      }
      case SM_BLOCK_EXPR: {
      return_obj    = (sm_object *)sms_true;
      sm_cx *new_cx = sm_new_cx(current_cx);
      for (uint32_t i = 1; i < sme->size && return_obj->my_type != SM_RETURN_TYPE; i++)
      sm_eval(sm_expr_get_arg(sme, i), new_cx, sf);
      return;
      break;
      }
      case SM_ASSIGN_EXPR: {
      sm_symbol *sym;
      sm_object *obj0 = sm_expr_get_arg(sme, 0);
      sm_eval(sm_expr_get_arg(sme, 1));
      sm_object *value = return_obj;
      if (!expect_type(obj0, SM_SYMBOL_TYPE))
      return(((sm_object *)sms_false));
      sym = (sm_symbol *)obj0;
      if (!sm_cx_set(current_cx, sym, value))
      return(((sm_object *)sms_false));
      return((value));
      }
      case SM_ASSIGN_DOT_EXPR: {
      sm_eval(sm_expr_get_arg(sme, 0));
      sm_cx     *predot  = (sm_cx *)return_obj;
      sm_symbol *postdot = (sm_symbol *)sm_expr_get_arg(sme, 1);
      sm_eval(sm_expr_get_arg(sme, 2));
      sm_object *value = return_obj;

      if (!sm_cx_let(predot, postdot, value))
      return(((sm_object *)sms_false));
      return(((sm_object *)sms_true));
      }
      case SM_ASSIGN_LOCAL_EXPR: {
      sm_object *obj0 = sm_expr_get_arg(sme, 0);
      sm_eval(sm_expr_get_arg(sme, 1));
      sm_object *value = (sm_object *)return_obj;
      if (obj0->my_type == SM_LOCAL_TYPE) {
      sm_local *lcl = (sm_local *)obj0;
      sm_expr_set_arg(sf, lcl->index, value);
      } else
      return(((sm_object *)sms_false));
      return((value));
      break;
      }


      case SM_ASSIGN_INDEX_EXPR: {
      // Expecting a[4]=value => =index_expr(a,4,value);
      sm_eval(sm_expr_get_arg(sme, 2));
      sm_object *value = return_obj;
      eager_type_check2(sme, 0, SM_EXPR_TYPE, SM_ARRAY_TYPE);
      sm_expr *arr_obj = (sm_expr *)return_obj;
      eager_type_check(sme, 1, SM_F64_TYPE);
      sm_f64 *index = (sm_f64 *)return_obj;
      // This works on both tuples and arrays
      switch (arr_obj->my_type) {
      case SM_EXPR_TYPE: {
      sm_expr *arr_expr = (sm_expr *)arr_obj;
      if ((uint32_t)index->value >= arr_expr->size || (uint32_t)index->value < 0) {
      sm_symbol *title = sm_new_symbol("arrayIndexOutOfBounds", 17);
      sm_string *message =
      sm_new_fstring_at(sms_heap, "Index %u is out of bounds of array of size %u",
                 (uint32_t)index->value, (uint32_t)arr_expr->size);
      return(((sm_object *)sm_new_error_from_expr(title, message, sme, NULL)));
      }
      sm_expr_set_arg(arr_expr, (uint32_t)index->value, value);
      break;
      }
      case SM_ARRAY_TYPE: {
      sm_array *arr = (sm_array *)arr_obj;
      if ((uint32_t)index->value >= arr->size || (uint32_t)index->value < 0) {
      sm_symbol *title = sm_new_symbol("arrayIndexOutOfBounds", 17);
      sm_string *message =
      sm_new_fstring_at(sms_heap, "Index %u is out of bounds of array of size %u",
                 (uint32_t)index->value, (uint32_t)arr->size);
      return(((sm_object *)sm_new_error_from_expr(title, message, sme, NULL)));
      }
      if (arr->inner_type != value->my_type) {
      sm_symbol *title   = sm_new_symbol("arrayAssignmentTypeMismatch", 27);
      sm_string *message = sm_new_fstring_at(
      sms_heap, "Array contains objects of type %s, but assignment value has type %s",
      sm_type_name(arr->inner_type), sm_type_name(value->my_type));
      return(((sm_object *)sm_new_error_from_expr(title, message, sme, NULL)));
      }
      switch (arr->inner_type) {
      case SM_F64_TYPE: {
      sm_f64_array_set(arr, index->value, ((sm_f64 *)value)->value);
      break;
      }
      case SM_UI8_TYPE: {
      sm_ui8_array_set(arr, index->value, ((sm_ui8 *)value)->value);
      break;
      }
      default: {
      sm_symbol *title   = sm_new_symbol("arrayTypeUnknownError", 19);
      sm_string *message = sm_new_fstring_at(sms_heap, "Unsupported array inner type %i",
                                    (uint32_t)arr->inner_type);
      return(((sm_object *)sm_new_error_from_expr(title, message, sme, NULL)));
      }
      }
      break;
      }
      default: {
      sm_symbol *title = sm_new_symbol("invalidExpressionType", 19);
      sm_string *message =
      sm_new_fstring_at(sms_heap, "Invalid expression type %i", (uint32_t)arr_obj->my_type);
      return(((sm_object *)sm_new_error_from_expr(title, message, sme, NULL)));
      }
      }
      return((value));
      }
      case SM_IXOR_EXPR: {
      eager_type_check(sme, 0, SM_UI8_TYPE);
      sm_ui8 *obj0 = (sm_ui8 *)return_obj;
      if (obj0->my_type != SM_UI8_TYPE)
      return(((sm_object *)obj0));
      eager_type_check(sme, 1, SM_UI8_TYPE);
      sm_ui8 *obj1 = (sm_ui8 *)return_obj;
      if (obj1->my_type != SM_UI8_TYPE)
      return(((sm_object *)obj1));
      return(((sm_object *)sm_new_ui8(obj0->value ^ obj1->value)));
      break;
      }
      case SM_IAND_EXPR: {
      eager_type_check(sme, 0, SM_UI8_TYPE);
      sm_ui8 *obj0 = (sm_ui8 *)return_obj;
      if (obj0->my_type != SM_UI8_TYPE)
      return(((sm_object *)obj0));
      eager_type_check(sme, 1, SM_UI8_TYPE);
      sm_ui8 *obj1 = (sm_ui8 *)return_obj;
      if (obj1->my_type != SM_UI8_TYPE)
      return(((sm_object *)obj1));
      return(((sm_object *)sm_new_ui8(obj0->value & obj1->value)));
      break;
      }
      case SM_IOR_EXPR: {
      eager_type_check(sme, 0, SM_UI8_TYPE);
      sm_ui8 *obj0 = (sm_ui8 *)return_obj;
      if (obj0->my_type != SM_UI8_TYPE)
      return(((sm_object *)obj0));
      eager_type_check(sme, 1, SM_UI8_TYPE);
      sm_ui8 *obj1 = (sm_ui8 *)return_obj;
      if (obj1->my_type != SM_UI8_TYPE)
      return(((sm_object *)obj1));
      return(((sm_object *)sm_new_ui8(obj0->value | obj1->value)));
      break;
      }
      case SM_IPLUSEQ_EXPR: {
      sm_symbol *sym = (sm_symbol *)sm_expr_get_arg(sme, 0);
      sm_eval(sm_expr_get_arg(sme, 1));
      sm_object *value         = return_obj;
      sm_object *current_value = sm_cx_get(current_cx, sym);
      if (!current_value || !sm_object_is_int(current_value) || !sm_object_is_int(value)) {
      sm_symbol *title = sm_new_symbol("invalidOperandTypes", 17);
      sm_string *message =
      sm_new_fstring_at(sms_heap, "Invalid types for += operation. Expected numbers.");
      return(((sm_object *)sm_new_error_from_expr(title, message, sme, NULL)));
      }
      sm_ui8 *current_ui8 = (sm_ui8 *)current_value;
      sm_ui8 *value_ui8   = (sm_ui8 *)value;
      if (current_ui8->my_type != SM_UI8_TYPE || value_ui8->my_type != SM_UI8_TYPE) {
      sm_symbol *title   = sm_new_symbol("invalidNumberType", 16);
      sm_string *message = sm_new_fstring_at(
      sms_heap, "Operands must be of type %s for !+= operation.", sm_type_name(SM_UI8_TYPE));
      return(((sm_object *)sm_new_error_from_expr(title, message, sme, NULL)));
      }
      sm_object *result = (sm_object *)sm_new_ui8(current_ui8->value + value_ui8->value);
      if (!sm_cx_set(current_cx, sym, result)) {
      sm_symbol *title   = sm_new_symbol("contextUpdateFailed", 19);
      sm_string *message = sm_new_fstring_at(sms_heap, "Failed to update symbol in context.");
      return(((sm_object *)sm_new_error_from_expr(title, message, sme, NULL)));
      }
      return((result));
      }
      case SM_IMINUSEQ_EXPR: {
      sm_symbol *sym = (sm_symbol *)sm_expr_get_arg(sme, 0);
      sm_eval(sm_expr_get_arg(sme, 1));
      sm_object *value         = return_obj;
      sm_object *current_value = sm_cx_get(current_cx, sym);
      if (!current_value || !sm_object_is_int(current_value) || !sm_object_is_int(value)) {
      sm_symbol *title = sm_new_symbol("invalidOperandTypes", 17);
      sm_string *message =
      sm_new_fstring_at(sms_heap, "Invalid types for !-= operation. Expected numbers.");
      return(((sm_object *)sm_new_error_from_expr(title, message, sme, NULL)));
      }
      sm_ui8 *current_ui8 = (sm_ui8 *)current_value;
      sm_ui8 *value_ui8   = (sm_ui8 *)value;
      if (current_ui8->my_type != SM_UI8_TYPE || value_ui8->my_type != SM_UI8_TYPE) {
      sm_symbol *title   = sm_new_symbol("invalidNumberType", 16);
      sm_string *message = sm_new_fstring_at(
      sms_heap, "Operands must be of type %s for -= operation.", sm_type_name(SM_UI8_TYPE));
      return(((sm_object *)sm_new_error_from_expr(title, message, sme, NULL)));
      }
      sm_object *result = (sm_object *)sm_new_ui8(current_ui8->value - value_ui8->value);
      if (!sm_cx_set(current_cx, sym, result)) {
      sm_symbol *title   = sm_new_symbol("contextUpdateFailed", 19);
      sm_string *message = sm_new_fstring_at(sms_heap, "Failed to update symbol in context.");
      return(((sm_object *)sm_new_error_from_expr(title, message, sme, NULL)));
      }
      return((result));
      }
      case SM_ITIMESEQ_EXPR: {
      sm_symbol *sym = (sm_symbol *)sm_expr_get_arg(sme, 0);
      sm_eval(sm_expr_get_arg(sme, 1));
      sm_object *value         = return_obj;
      sm_object *current_value = sm_cx_get(current_cx, sym);
      if (!current_value || !sm_object_is_int(current_value) || !sm_object_is_int(value)) {
      sm_symbol *title = sm_new_symbol("invalidOperandTypes", 17);
      sm_string *message =
      sm_new_fstring_at(sms_heap, "Invalid types for *= operation. Expected numbers.");
      return(((sm_object *)sm_new_error_from_expr(title, message, sme, NULL)));
      }
      sm_ui8 *current_ui8 = (sm_ui8 *)current_value;
      sm_ui8 *value_ui8   = (sm_ui8 *)value;
      if (current_ui8->my_type != SM_UI8_TYPE || value_ui8->my_type != SM_UI8_TYPE) {
      sm_symbol *title   = sm_new_symbol("invalidNumberType", 16);
      sm_string *message = sm_new_fstring_at(
      sms_heap, "Operands must be of type %s for *= operation.", sm_type_name(SM_UI8_TYPE));
      return(((sm_object *)sm_new_error_from_expr(title, message, sme, NULL)));
      }
      sm_object *result = (sm_object *)sm_new_ui8(current_ui8->value * value_ui8->value);
      if (!sm_cx_set(current_cx, sym, result)) {
      sm_symbol *title   = sm_new_symbol("contextUpdateFailed", 19);
      sm_string *message = sm_new_fstring_at(sms_heap, "Failed to update symbol in context.");
      return(((sm_object *)sm_new_error_from_expr(title, message, sme, NULL)));
      }
      return((result));
      }
      case SM_IDIVIDEEQ_EXPR: {
      sm_symbol *sym = (sm_symbol *)sm_expr_get_arg(sme, 0);
      sm_eval(sm_expr_get_arg(sme, 1));
      sm_object *value         = return_obj;
      sm_object *current_value = sm_cx_get(current_cx, sym);
      if (!current_value || !sm_object_is_int(current_value) || !sm_object_is_int(value)) {
      sm_symbol *title = sm_new_symbol("invalidOperandTypes", 17);
      sm_string *message =
      sm_new_fstring_at(sms_heap, "Invalid types for /= operation. Expected numbers.");
      return(((sm_object *)sm_new_error_from_expr(title, message, sme, NULL)));
      }
      sm_ui8 *current_ui8 = (sm_ui8 *)current_value;
      sm_ui8 *value_ui8   = (sm_ui8 *)value;
      if (current_ui8->my_type != SM_UI8_TYPE || value_ui8->my_type != SM_UI8_TYPE) {
      sm_symbol *title   = sm_new_symbol("invalidNumberType", 16);
      sm_string *message = sm_new_fstring_at(
      sms_heap, "Operands must be of type %s for /= operation.", sm_type_name(SM_UI8_TYPE));
      return(((sm_object *)sm_new_error_from_expr(title, message, sme, NULL)));
      }
      if (value_ui8->value == 0.0) {
      sm_symbol *title   = sm_new_symbol("divisionByZero", 15);
      sm_string *message = sm_new_fstring_at(sms_heap, "Division by zero in /= operation.");
      return(((sm_object *)sm_new_error_from_expr(title, message, sme, NULL)));
      }
      sm_object *result = (sm_object *)sm_new_ui8(current_ui8->value / value_ui8->value);
      if (!sm_cx_set(current_cx, sym, result)) {
      sm_symbol *title   = sm_new_symbol("contextUpdateFailed", 19);
      sm_string *message = sm_new_fstring_at(sms_heap, "Failed to update symbol in context.");
      return(((sm_object *)sm_new_error_from_expr(title, message, sme, NULL)));
      }
      return((result));
      }

      */

    case SM_PLUS_EXPR: {
      sm_push(sm_eval(sm_expr_get_arg(sme, 1)));
      sm_push(sm_eval(sm_expr_get_arg(sme, 0)));
      return sm_add();
    }
    case SM_MINUS_EXPR: {
      sm_push(sm_eval(sm_expr_get_arg(sme, 1)));
      sm_push(sm_eval(sm_expr_get_arg(sme, 0)));
      return sm_minus();
    }
    case SM_TIMES_EXPR: {
      sm_push(sm_eval(sm_expr_get_arg(sme, 1)));
      sm_push(sm_eval(sm_expr_get_arg(sme, 0)));
      return sm_times();
    }
    case SM_DIVIDE_EXPR: {
      sm_push(sm_eval(sm_expr_get_arg(sme, 1)));
      sm_push(sm_eval(sm_expr_get_arg(sme, 0)));
      return sm_divide();
    }
    }
  }
  }
  return input;
}
