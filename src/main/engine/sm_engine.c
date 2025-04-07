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
extern sm_object   *return_obj;
extern sm_stack2   *sms_stack;
extern sm_stack2   *sms_cx_stack;

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
  return_obj     = sm_expr_get_arg(sme, operand);
  sm_object *obj = sm_eval(return_obj);
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
  return_obj     = sm_expr_get_arg(sme, operand);
  sm_object *obj = sm_eval(return_obj);
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
      sm_object *fromObj;
      eager_type_check3(sme, 0, SM_UI8_TYPE, SM_F64_TYPE, SM_STRING_TYPE);
      fromObj = return_obj;
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
      sm_string *fname_str;
      eager_type_check(sme, 0, SM_STRING_TYPE);
      fname_str = (sm_string *)return_obj;
      if (fname_str->my_type == SM_ERR_TYPE)
        return (((sm_object *)fname_str)); // return(the error if type check fails);
      char *fname_cstr = &(fname_str->content);
      return ((sm_object *)sm_new_f64(sm_mem_dump(sms_heap, fname_cstr)));
    }

    case SM_IMAGESAVE_EXPR: {
      // Obtain the file name using eager_type_check
      sm_string *fname_str;
      eager_type_check(sme, 0, SM_STRING_TYPE);
      fname_str = (sm_string *)return_obj;
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
      eager_type_check(sme, 0, SM_F64_TYPE);
      sm_f64 *obj0 = (sm_f64 *)return_obj;
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
      eager_type_check(sme, 0, SM_STRING_TYPE);
      sm_string *path = (sm_string *)return_obj;
      if (path->my_type == SM_ERR_TYPE)
        return (((sm_object *)path));
      // The system command leaves 8 bits for extra information
      // We do not need it, so we shift away the 8 bits
      uint32_t result = system(&path->content) >> 8;
      return (((sm_object *)sm_new_f64(result)));
      break;
    }
    case SM_EXECTOSTR_EXPR: {
      eager_type_check(sme, 0, SM_STRING_TYPE);
      sm_string *path = (sm_string *)return_obj;
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
      eager_type_check(sme, 0, SM_STRING_TYPE);
      sm_string *key = (sm_string *)return_obj;
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
      eager_type_check(sme, 0, SM_STRING_TYPE);
      sm_string *key = (sm_string *)return_obj;
      if (key->my_type == SM_ERR_TYPE)
        return (((sm_object *)key));
      eager_type_check(sme, 1, SM_STRING_TYPE);
      sm_string *value = (sm_string *)return_obj;
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
      eager_type_check(sme, 0, SM_STRING_TYPE);
      sm_string *path = (sm_string *)return_obj;
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
      eager_type_check(sme, 0, SM_STRING_TYPE);
      sm_string *haystack = (sm_string *)return_obj;
      if (haystack->my_type == SM_ERR_TYPE)
        return (((sm_object *)haystack));
      eager_type_check(sme, 1, SM_STRING_TYPE);
      sm_string *needle = (sm_string *)return_obj;
      if (needle->my_type == SM_ERR_TYPE)
        return (((sm_object *)needle));
      return ((sm_str_find(haystack, needle)));
      break;
    }
    case SM_STR_FINDR_EXPR: {
      eager_type_check(sme, 0, SM_STRING_TYPE);
      sm_string *haystack = (sm_string *)return_obj;
      if (haystack->my_type == SM_ERR_TYPE)
        return (((sm_object *)haystack));
      eager_type_check(sme, 1, SM_STRING_TYPE);
      sm_string *needle = (sm_string *)return_obj;
      if (needle->my_type == SM_ERR_TYPE)
        return (((sm_object *)needle));
      return ((sm_str_findr(haystack, needle)));
      break;
    }
    case SM_STR_SPLIT_EXPR: {
      eager_type_check(sme, 0, SM_STRING_TYPE);
      sm_string *haystack = (sm_string *)return_obj;
      if (haystack->my_type == SM_ERR_TYPE)
        return (((sm_object *)haystack));
      eager_type_check(sme, 1, SM_STRING_TYPE);
      sm_string *needle = (sm_string *)return_obj;
      if (needle->my_type == SM_ERR_TYPE)
        return (((sm_object *)needle));
      return (((sm_object *)sm_str_split(haystack, needle)));
      break;
    }
    case SM_STR_PART_EXPR: {
      eager_type_check(sme, 0, SM_STRING_TYPE);
      sm_string *str0 = (sm_string *)return_obj;
      if (str0->my_type == SM_ERR_TYPE)
        return (((sm_object *)str0));
      eager_type_check(sme, 1, SM_F64_TYPE);
      sm_f64 *start = (sm_f64 *)return_obj;
      if (start->my_type == SM_ERR_TYPE)
        return (((sm_object *)start));
      eager_type_check(sme, 2, SM_F64_TYPE);
      sm_f64 *len = (sm_f64 *)return_obj;
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
      eager_type_check(sme, 0, SM_STRING_TYPE);
      sm_string *original_str = (sm_string *)return_obj;
      if (original_str->my_type == SM_ERR_TYPE)
        return (((sm_object *)original_str));
      eager_type_check(sme, 1, SM_F64_TYPE);
      sm_f64 *start_index = (sm_f64 *)return_obj;
      if (start_index->my_type == SM_ERR_TYPE)
        return (((sm_object *)start_index));
      eager_type_check(sme, 2, SM_STRING_TYPE);
      sm_string *replacement_str = (sm_string *)return_obj;
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
      eager_type_check(sme, 0, SM_STRING_TYPE);
      sm_string *str0 = (sm_string *)return_obj;
      if (str0->my_type != SM_STRING_TYPE)
        return (((sm_object *)str0));
      eager_type_check(sme, 1, SM_STRING_TYPE);
      sm_string *str1 = (sm_string *)return_obj;
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
      eager_type_check(sme, 0, SM_STRING_TYPE);
      sm_string *str0 = (sm_string *)return_obj;
      if (str0->my_type == SM_ERR_TYPE)
        return (((sm_object *)str0));
      return (((sm_object *)sm_new_f64(str0->size)));
      break;
    }
    case SM_STR_REPEAT_EXPR: {
      eager_type_check(sme, 0, SM_STRING_TYPE);
      sm_string *str = (sm_string *)return_obj;
      if (str->my_type == SM_ERR_TYPE)
        return (((sm_object *)str));
      eager_type_check(sme, 1, SM_F64_TYPE);
      sm_f64 *reps = (sm_f64 *)return_obj;
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
      eager_type_check(sme, 0, SM_F64_TYPE);
      sm_f64 *num0 = (sm_f64 *)return_obj;
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
      eager_type_check(sme, 0, SM_EXPR_TYPE);
      sm_expr *list0 = (sm_expr *)return_obj;
      if (list0->my_type == SM_ERR_TYPE)
        return (((sm_object *)list0));
      eager_type_check(sme, 1, SM_F64_TYPE);
      sm_f64 *start = (sm_f64 *)return_obj;
      if (start->my_type == SM_ERR_TYPE)
        return (((sm_object *)start));
      eager_type_check(sme, 2, SM_F64_TYPE);
      sm_f64 *len = (sm_f64 *)return_obj;
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
    }
  }
  }
  return input;
}
