// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"
#include "sys/time.h"
#include <dirent.h>
#include <sys/stat.h>
#include <sys/wait.h>

// Globals from sm_global.c
extern sm_heap   *sms_heap;
extern sm_heap   *sms_other_heap;
extern sm_symbol *sms_true;
extern sm_symbol *sms_false;

// Execute a function
inline sm_object *execute_fun(sm_fun *fun, sm_cx *current_cx, sm_expr *sf) {
  sm_object *content = fun->content;
  sm_object *result;
  sm_cx     *new_cx = sm_new_cx(fun->parent);
  if (content->my_type == SM_EXPR_TYPE && ((sm_expr *)content)->op == SM_BLOCK_EXPR) {
    sm_expr *content_sme = (sm_expr *)fun->content;
    uint32_t i           = 1;
    while (i < content_sme->size) {
      result = sm_engine_eval(sm_expr_get_arg(content_sme, i), new_cx, sf);
      if (result->my_type == SM_RETURN_TYPE)
        return ((sm_return *)result)->address;
      i++;
    }
    return result;
  } else {
    return sm_engine_eval(content, new_cx, sf);
  }
}

// Basic type checking
static inline bool expect_type(sm_object *arg_n, int16_t arg_type) {
  return arg_n->my_type == arg_type;
}

// Returns the object if it's ok, returns an error if it's not
static inline sm_object *type_check(sm_expr *sme, uint32_t operand, int param_type) {
  sm_object *obj = sm_expr_get_arg(sme, operand);
  if (param_type != obj->my_type) {
    sm_string *source  = (sm_string *)sm_cx_get(sme->notes, sm_new_symbol("source", 6));
    sm_double *line    = (sm_double *)sm_cx_get(sme->notes, sm_new_symbol("line", 4));
    sm_string *message = sm_new_fstring_at(
      sms_heap, "Wrong type for argument %i on %s. Argument type is: %s , but Expected: %s",
      operand, sm_global_fn_name(sme->op), sm_global_type_name(obj->my_type),
      sm_global_type_name(param_type));
    // evaluate error handler if there is one
    sm_cx *scratch = (sm_cx *)*(sm_global_lex_stack(NULL)->top);
    return (sm_object *)sm_new_error(12, "typeMismatch", message->size, &message->content,
                                     source->size, &source->content, (int)line->value);
  }
  return obj;
}

// Evaluate the argument, then run type check
static inline sm_object *eager_type_check(sm_expr *sme, int operand, int param_type,
                                          sm_cx *current_cx, sm_expr *sf) {
  sm_object *obj = sm_engine_eval(sm_expr_get_arg(sme, operand), current_cx, sf);
  if (param_type != obj->my_type) {
    sm_string *source  = (sm_string *)sm_cx_get(sme->notes, sm_new_symbol("source", 6));
    sm_double *line    = (sm_double *)sm_cx_get(sme->notes, sm_new_symbol("line", 4));
    sm_string *message = sm_new_fstring_at(
      sms_heap, "Wrong type for argument %i on %s. Argument type is: %s , but Expected: %s",
      operand, sm_global_fn_name(sme->op), sm_global_type_name(obj->my_type),
      sm_global_type_name(param_type));
    return (sm_object *)sm_new_error(12, "typeMismatch", message->size, &message->content,
                                     source->size, &source->content, (int)line->value);
  }
  return obj;
}

// Convenience functions for the booleans
#define IS_TRUE(x) ((void *)x == (void *)sms_true)
#define IS_FALSE(x) ((void *)x == (void *)sms_false)

// Recursive engine
inline sm_object *sm_engine_eval(sm_object *input, sm_cx *current_cx, sm_expr *sf) {
  switch (input->my_type) {
  case SM_EXPR_TYPE: {
    sm_expr *sme = (sm_expr *)input;
    short    op  = sme->op;
    switch (op) {
    case SM_DATE_EXPR: {
      time_t     rawtime;
      struct tm *timeinfo;
      time(&rawtime);
      timeinfo            = localtime(&rawtime);
      int     *time_array = (int *)timeinfo;
      sm_expr *result     = sm_new_expr_n(SM_ARRAY_EXPR, 9, 9, NULL);
      for (int i = 0; i < 9; i++)
        sm_expr_set_arg(result, i, (sm_object *)sm_new_double(time_array[i]));
      return (sm_object *)result;
      break;
    }
    case SM_GC_EXPR: {
      // This fails because it changes all of the pointers before the function returns.
      // sm_garbage_collect();
      sm_symbol *title   = sm_new_symbol("notImplemented", 14);
      sm_string *message = sm_new_string(33, "_gc() command is not implemented yet");
      return (sm_object *)sm_new_error_from_expr(title, message, sme, NULL);
      break;
    }
    case SM_SLEEP_EXPR: {
      sm_double *obj0 = (sm_double *)eager_type_check(sme, 0, SM_DOUBLE_TYPE, current_cx, sf);
      if (obj0->my_type != SM_DOUBLE_TYPE) {
        sm_symbol *title   = sm_new_symbol("nonNumericTime", 14);
        sm_string *message = sm_new_string(48, "sleep function was provided a non-numeric value.");
        return (sm_object *)sm_new_error_from_expr(title, message, sme, NULL);
      }
      int tms;
      tms = (int)((sm_double *)obj0)->value;
      if (tms < 0) {
        sm_symbol *title   = sm_new_symbol("negativeTime", 12);
        sm_string *message = sm_new_string(45, "sleep function was provided a negative value.");
        return (sm_object *)sm_new_error_from_expr(title, message, sme, NULL);
      }
      struct timespec ts;
      int             ret;
      ts.tv_sec  = tms / 1000;
      ts.tv_nsec = (tms % 1000) * 1000000;
      do {
        ret = nanosleep(&ts, &ts);
      } while (ret);
      return (sm_object *)sms_true;
      break;
    }
    case SM_FORK_EXPR: {
      pid_t pid = fork();
      return (sm_object *)sm_new_double(pid);
    }
    case SM_WAIT_EXPR: {
      int status;
      int child_pid = wait(&status);
      status        = WEXITSTATUS(status);
      if (child_pid == -1)
        status = 1;
      return (sm_object *)sm_new_expr_2(SM_ARRAY_EXPR, (sm_object *)sm_new_double(child_pid),
                                        (sm_object *)sm_new_double(status), NULL);
    }
    case SM_EXEC_EXPR: {
      sm_string *path = (sm_string *)eager_type_check(sme, 0, SM_STRING_TYPE, current_cx, sf);
      if (path->my_type == SM_ERR_TYPE)
        return (sm_object *)path;
      // The system command leaves 8 bits for extra information
      // We do not need it, so we shift away the 8 bits
      int result = system(&path->content) >> 8;
      return (sm_object *)sm_new_double(result);
      break;
    }
    case SM_EXECTOSTR_EXPR: {
      sm_string *path = (sm_string *)eager_type_check(sme, 0, SM_STRING_TYPE, current_cx, sf);
      if (path->my_type == SM_ERR_TYPE)
        return (sm_object *)path;
      FILE      *fp;
      char       buffer[128]; // Buffer size to read the command output in chunks
      sm_symbol *title       = sm_new_symbol("osExecToStrFailed", 11);
      char      *output_data = NULL;
      size_t     total_size  = 0;
      // Execute the command and open a pipe to read its output
      fp = popen(&(path->content), "r");
      if (fp == NULL) {
        sm_string *message = sm_new_string(45, "Failed to open pipe for command execution.");
        return (sm_object *)sm_new_error_from_expr(title, message, sme, NULL);
      }
      // Read the command output in chunks
      while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        size_t buffer_len      = strlen(buffer);
        char  *new_output_data = realloc(output_data, total_size + buffer_len + 1);
        if (new_output_data == NULL) {
          free(output_data);
          sm_string *message = sm_new_string(47, "Failed to allocate memory for command output.");
          return (sm_object *)sm_new_error_from_expr(title, message, sme, NULL);
        }
        output_data = new_output_data;
        memcpy(output_data + total_size, buffer, buffer_len);
        total_size += buffer_len;
        output_data[total_size] = '\0'; // Null terminate
      }
      // Close the pipe and get the command exit status
      int return_code = pclose(fp) >> 8;
      // Create a new sm_string with the collected output and free the output data
      sm_string *result_str = sm_new_string(total_size, output_data);
      free(output_data);
      // Return a size 2 array with return value and return string
      sm_double *output_code = sm_new_double(return_code);
      sm_expr   *output =
        sm_new_expr_2(SM_ARRAY_EXPR, (sm_object *)output_code, (sm_object *)result_str, NULL);
      return (sm_object *)output;
      break;
    }

    case SM_OS_GETENV_EXPR: {
      sm_string *key = (sm_string *)eager_type_check(sme, 0, SM_STRING_TYPE, current_cx, sf);
      if (key->my_type == SM_ERR_TYPE)
        return (sm_object *)key;
      char *result = getenv(&key->content);
      if (!result) {
        sm_symbol *title = sm_new_symbol("osGetEnvFailed", 14);
        sm_string *message =
          sm_new_fstring_at(sms_heap, "Failed to get environment variable: %s", &key->content);
        return (sm_object *)sm_new_error_from_expr(title, message, sme, NULL);
      }
      return (sm_object *)sm_new_string(strlen(result), result);
      break;
    }
    case SM_OS_SETENV_EXPR: {
      sm_string *key = (sm_string *)eager_type_check(sme, 0, SM_STRING_TYPE, current_cx, sf);
      if (key->my_type == SM_ERR_TYPE)
        return (sm_object *)key;
      sm_string *value = (sm_string *)eager_type_check(sme, 1, SM_STRING_TYPE, current_cx, sf);
      if (value->my_type == SM_ERR_TYPE)
        return (sm_object *)value;
      int result = setenv(&key->content, &value->content, 1);
      return (sm_object *)sm_new_double(result);
      break;
    }
    case SM_LS_EXPR: {
      DIR           *dir;
      struct dirent *entry;
      struct stat    file_stat;
      const uint32_t MAX_ENTRIES = 1000;
      sm_string     *entry_names[MAX_ENTRIES];
      bool           entry_types[MAX_ENTRIES];
      uint32_t       i = 0;

      char cwd[1024];
      if (getcwd(cwd, sizeof(cwd))) {
      } else {
        printf("Error: Current working directory is invalid: %s .\n", cwd);
        return (sm_object *)sm_new_double(0);
      }

      dir = opendir(cwd);
      if (dir == NULL) {
        printf("Error: Current working directory is invalid: %s .\n", cwd);
        return (sm_object *)sm_new_double(0);
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
      sm_expr *names_arr = sm_new_expr_n(SM_ARRAY_EXPR, i, i, NULL);
      sm_expr *types_arr = sm_new_expr_n(SM_ARRAY_EXPR, i, i, NULL);
      for (uint32_t names_i = 0; names_i < i; names_i++) {
        sm_expr_set_arg(names_arr, names_i, (sm_object *)entry_names[names_i]);
        if (entry_types[names_i] != 0)
          sm_expr_set_arg(types_arr, names_i, (sm_object *)sms_true);
        else
          sm_expr_set_arg(types_arr, names_i, (sm_object *)sms_false);
      }
      sm_expr *result =
        sm_new_expr_2(SM_ARRAY_EXPR, (sm_object *)names_arr, (sm_object *)types_arr, NULL);
      return (sm_object *)result;
      break;
    }
    case SM_PWD_EXPR: {
      char cwd[1024];
      if (getcwd(cwd, sizeof(cwd))) {
      } else {
        printf("Error: Current working directory is invalid: %s .\n", cwd);
        return (sm_object *)sm_new_double(0);
      }
      return (sm_object *)sm_new_string(strlen(cwd), cwd);
      break;
    }
    case SM_CD_EXPR: {
      sm_string *path = (sm_string *)eager_type_check(sme, 0, SM_STRING_TYPE, current_cx, sf);
      if (path->my_type != SM_STRING_TYPE)
        return (sm_object *)path;
      char *path_cstr = &path->content;
      if (chdir(path_cstr) == 0)
        return (sm_object *)sms_true;
      sm_symbol *title   = sm_new_symbol("cdFailed", 8);
      sm_string *message = sm_new_fstring_at(sms_heap, "Failed to change directory to %s");
      return (sm_object *)sm_new_error_from_expr(title, message, sme, NULL);
      break;
    }
    case SM_DATE_STR_EXPR: {
      time_t     rawtime;
      struct tm *timeinfo;
      time(&rawtime);
      timeinfo          = localtime(&rawtime);
      sm_string *result = sm_new_string_manual(24);
      sm_strncpy(&(result->content), asctime(timeinfo), 24);
      return (sm_object *)result;
      break;
    }
    case SM_TIME_EXPR: {
      struct timeval t;
      gettimeofday(&t, NULL);
      sm_expr *result = sm_new_expr_n(SM_ARRAY_EXPR, 2, 2, NULL);
      sm_expr_set_arg(result, 0, (sm_object *)sm_new_double(t.tv_sec));
      sm_expr_set_arg(result, 1, (sm_object *)sm_new_double(t.tv_usec));
      return (sm_object *)result;
      break;
    }
    case SM_STR_FIND_EXPR: {
      sm_string *haystack = (sm_string *)eager_type_check(sme, 0, SM_STRING_TYPE, current_cx, sf);
      if (haystack->my_type == SM_ERR_TYPE)
        return (sm_object *)haystack;
      sm_string *needle = (sm_string *)eager_type_check(sme, 1, SM_STRING_TYPE, current_cx, sf);
      if (needle->my_type == SM_ERR_TYPE)
        return (sm_object *)needle;

      return (sm_object *)sm_str_find(haystack, needle);
      break;
    }
    case SM_STR_SPLIT_EXPR: {
      sm_string *haystack = (sm_string *)eager_type_check(sme, 0, SM_STRING_TYPE, current_cx, sf);
      if (haystack->my_type == SM_ERR_TYPE)
        return (sm_object *)haystack;
      sm_string *needle = (sm_string *)eager_type_check(sme, 1, SM_STRING_TYPE, current_cx, sf);
      if (needle->my_type == SM_ERR_TYPE)
        return (sm_object *)needle;
      return (sm_object *)sm_str_split(haystack, needle);
      break;
    }
    case SM_STR_PART_EXPR: {
      sm_string *str0 = (sm_string *)eager_type_check(sme, 0, SM_STRING_TYPE, current_cx, sf);
      if (str0->my_type == SM_ERR_TYPE)
        return (sm_object *)str0;
      sm_double *start = (sm_double *)eager_type_check(sme, 1, SM_DOUBLE_TYPE, current_cx, sf);
      if (start->my_type == SM_ERR_TYPE)
        return (sm_object *)start;
      sm_double *len = (sm_double *)eager_type_check(sme, 2, SM_DOUBLE_TYPE, current_cx, sf);
      if (len->my_type == SM_ERR_TYPE)
        return (sm_object *)len;
      if (start->value < 0 || start->value >= str0->size) {
        sm_symbol *title   = sm_new_symbol("strPartIndexErr", 15);
        sm_string *message = sm_new_fstring_at(
          sms_heap, "Calling strPart with out of range start value: %i", (int)start->value);
        return (sm_object *)sm_new_error_from_expr(title, message, sme, NULL);
      }
      if (len->value > str0->size - start->value) {
        sm_symbol *title   = sm_new_symbol("strPartLengthErr", 16);
        sm_string *message = sm_new_fstring_at(
          sms_heap, "Calling strPart with out of range length value: %i", (int)len->value);
        return (sm_object *)sm_new_error_from_expr(title, message, sme, NULL);
      }
      sm_string *new_str = sm_new_string_manual((int)len->value);
      char      *content = &(new_str->content);
      sm_strncpy(content, &(str0->content) + (int)start->value, (int)len->value);
      return (sm_object *)new_str;
      break;
    }
    case SM_STR_MUT_EXPR: {
      sm_string *original_str =
        (sm_string *)eager_type_check(sme, 0, SM_STRING_TYPE, current_cx, sf);
      if (original_str->my_type == SM_ERR_TYPE)
        return (sm_object *)original_str;
      sm_double *start_index =
        (sm_double *)eager_type_check(sme, 1, SM_DOUBLE_TYPE, current_cx, sf);
      if (start_index->my_type == SM_ERR_TYPE)
        return (sm_object *)start_index;
      sm_string *replacement_str =
        (sm_string *)eager_type_check(sme, 2, SM_STRING_TYPE, current_cx, sf);
      if (replacement_str->my_type == SM_ERR_TYPE)
        return (sm_object *)replacement_str;
      // Calculate cutout length
      int cutout_length = replacement_str->size;
      // Check if new size exceeds original size
      if (original_str->size - (int)start_index->value < (int)replacement_str->size) {
        sm_symbol *title   = sm_new_symbol("strMutOverflow", 14);
        sm_string *message = sm_new_string(46, "Could not mutate string within the size limit.");
        return (sm_object *)sm_new_error_from_expr(title, message, sme, NULL);
      }
      char *content = &(original_str->content);
      // Copy replacement string into the original string
      sm_strncpy_unsafe(content + (int)start_index->value, &(replacement_str->content),
                        replacement_str->size);
      // No need to copy the remainder since the string size stays the same
      return (sm_object *)original_str;
      break;
    }
    case SM_STR_CAT_EXPR: {
      sm_string *str0 = (sm_string *)eager_type_check(sme, 0, SM_STRING_TYPE, current_cx, sf);
      if (str0->my_type == SM_ERR_TYPE)
        return (sm_object *)str0;
      sm_string *str1 = (sm_string *)eager_type_check(sme, 1, SM_STRING_TYPE, current_cx, sf);
      if (str1->my_type == SM_ERR_TYPE)
        return (sm_object *)str1;
      sm_string *new_str = sm_new_string_manual(str0->size + str1->size);
      char      *content = &(new_str->content);
      sm_strncpy(content, &(str0->content), str0->size);
      sm_strncpy(content + str0->size, &(str1->content), str1->size);
      return (sm_object *)new_str;
      break;
    }
    case SM_STR_SIZE_EXPR: {
      sm_string *str0 = (sm_string *)eager_type_check(sme, 0, SM_STRING_TYPE, current_cx, sf);
      if (str0->my_type == SM_ERR_TYPE)
        return (sm_object *)str0;
      return (sm_object *)sm_new_double(str0->size);
      break;
    }
    case SM_ZEROS_EXPR: {
      sm_double *num0 = (sm_double *)eager_type_check(sme, 0, SM_DOUBLE_TYPE, current_cx, sf);
      if (num0->my_type != SM_DOUBLE_TYPE)
        return (sm_object *)num0;
      if (num0->value < 1)
        return (sm_object *)sm_new_expr_0(SM_ARRAY_EXPR, NULL);
      sm_double *zero   = sm_new_double(0);
      sm_expr   *output = sm_new_expr_n(SM_ARRAY_EXPR, (int)num0->value, (int)num0->value, NULL);
      for (int i = 0; i < num0->value; i++)
        sm_expr_set_arg(output, i, (sm_object *)zero);
      return (sm_object *)output;
      break;
    }
    case SM_PART_EXPR: {
      sm_expr *list0 = (sm_expr *)eager_type_check(sme, 0, SM_EXPR_TYPE, current_cx, sf);
      if (list0->my_type == SM_ERR_TYPE)
        return (sm_object *)list0;
      sm_double *start = (sm_double *)eager_type_check(sme, 1, SM_DOUBLE_TYPE, current_cx, sf);
      if (start->my_type == SM_ERR_TYPE)
        return (sm_object *)start;
      sm_double *len = (sm_double *)eager_type_check(sme, 2, SM_DOUBLE_TYPE, current_cx, sf);
      if (len->my_type == SM_ERR_TYPE)
        return (sm_object *)len;
      if (start->value < 0 || start->value >= list0->size) {
        sm_symbol *title   = sm_new_symbol("partIndexErr", 12);
        sm_string *message = sm_new_fstring_at(
          sms_heap, "Calling part with out of range start value: %i", (int)start->value);
        return (sm_object *)sm_new_error_from_expr(title, message, sme, NULL);
      }
      if (len->value > list0->size - start->value) {
        sm_symbol *title   = sm_new_symbol("partLengthErr", 13);
        sm_string *message = sm_new_fstring_at(
          sms_heap, "Calling part with out of range length value: %i", (int)len->value);
        return (sm_object *)sm_new_error_from_expr(title, message, sme, NULL);
      }
      sm_expr *new_list = sm_new_expr_n(SM_ARRAY_EXPR, (int)len->value, (int)len->value, NULL);
      for (int i = 0; i < (int)len->value; i++) {
        sm_object *element = sm_expr_get_arg(list0, (int)start->value + i);
        sm_expr_set_arg(new_list, i, element);
      }
      return (sm_object *)new_list;
      break;
    }
    case SM_REPEAT_EXPR: {
      sm_expr *list0 = (sm_expr *)eager_type_check(sme, 0, SM_EXPR_TYPE, current_cx, sf);
      if (list0->my_type == SM_ERR_TYPE)
        return (sm_object *)list0;
      sm_double *reps = (sm_double *)eager_type_check(sme, 1, SM_DOUBLE_TYPE, current_cx, sf);
      if (reps->my_type == SM_ERR_TYPE)
        return (sm_object *)reps;
      double   repetitions   = reps->value;
      int      original_size = list0->size;
      int      new_size      = (int)(original_size * repetitions);
      sm_expr *new_array     = sm_new_expr_n(SM_ARRAY_EXPR, new_size, new_size, NULL);
      for (int i = 0; i < new_size; i++) {
        int        original_index = i % original_size;
        sm_object *element        = sm_expr_get_arg(list0, original_index);
        sm_expr_set_arg(new_array, i, element);
      }
      return (sm_object *)new_array;
      break;
    }
    case SM_EXIT_EXPR: {
      int exit_code = 0;
      if (sme->size != 0) {
        sm_double *num0 = (sm_double *)eager_type_check(sme, 0, SM_DOUBLE_TYPE, current_cx, sf);
        if (num0->my_type == SM_ERR_TYPE)
          return (sm_object *)num0;
        exit_code = num0->value;
      }
      sm_signal_exit(exit_code);
      break;
    }
    case SM_LET_EXPR: {
      // Trust the parser for now regarding element 0 being a symbol
      sm_symbol *sym   = (sm_symbol *)sm_expr_get_arg(sme, 0);
      sm_object *value = (sm_object *)sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      // If an error occurred, it is stored in the mapping
      sm_cx_let(current_cx, sym, value);
      return value;
    }
    case SM_CX_SETPARENT_EXPR: {
      sm_cx *cx = (sm_cx *)eager_type_check(sme, 0, SM_CX_TYPE, current_cx, sf);
      if (cx->my_type == SM_ERR_TYPE)
        return (sm_object *)cx;
      sm_cx *new_parent = (sm_cx *)eager_type_check(sme, 1, SM_CX_TYPE, current_cx, sf);
      if (new_parent->my_type == SM_ERR_TYPE)
        return (sm_object *)new_parent;
      cx         = (sm_cx *)sm_copy((sm_object *)cx);
      cx->parent = new_parent;
      return (sm_object *)cx;
    }
    case SM_CX_LET_EXPR: {
      sm_cx *cx = (sm_cx *)eager_type_check(sme, 0, SM_CX_TYPE, current_cx, sf);
      if (cx->my_type == SM_ERR_TYPE)
        return (sm_object *)cx;
      sm_symbol *sym = (sm_symbol *)eager_type_check(sme, 1, SM_SYMBOL_TYPE, current_cx, sf);
      if (sym->my_type == SM_ERR_TYPE)
        return (sm_object *)sym;
      sm_object *value = (sm_object *)sm_engine_eval(sm_expr_get_arg(sme, 2), current_cx, sf);
      sm_cx_let(cx, sym, value);
      return value;
    }
    case SM_CX_GET_EXPR: {
      sm_cx *cx = (sm_cx *)eager_type_check(sme, 0, SM_CX_TYPE, current_cx, sf);
      if (cx->my_type == SM_ERR_TYPE)
        return (sm_object *)cx;
      sm_symbol *sym = (sm_symbol *)eager_type_check(sme, 1, SM_SYMBOL_TYPE, current_cx, sf);
      if (sym->my_type == SM_ERR_TYPE)
        return (sm_object *)sym;
      sm_object *result = sm_cx_get(cx, sym);
      if (result)
        return result;
      sm_symbol *title = sm_new_symbol("cxGetFailed", 11);
      sm_string *message =
        sm_new_fstring_at(sms_heap, "cxGet did not find %s in this context.", &sym->name->content);
      return (sm_object *)sm_new_error_from_expr(title, message, sme, NULL);
    }
    case SM_CX_HAS_EXPR: {
      sm_cx *cx = (sm_cx *)eager_type_check(sme, 0, SM_CX_TYPE, current_cx, sf);
      if (cx->my_type == SM_ERR_TYPE)
        return (sm_object *)cx;
      sm_symbol *sym = (sm_symbol *)eager_type_check(sme, 1, SM_SYMBOL_TYPE, current_cx, sf);
      if (sym->my_type == SM_ERR_TYPE)
        return (sm_object *)sym;
      sm_object *result = sm_cx_get(cx, sym);
      if (result)
        return (sm_object *)sms_true;
      return (sm_object *)sms_false;
    }
    case SM_CX_GET_FAR_EXPR: {
      sm_cx *cx = (sm_cx *)eager_type_check(sme, 0, SM_CX_TYPE, current_cx, sf);
      if (cx->my_type == SM_ERR_TYPE)
        return (sm_object *)cx;
      sm_symbol *sym = (sm_symbol *)eager_type_check(sme, 1, SM_SYMBOL_TYPE, current_cx, sf);
      if (sym->my_type == SM_ERR_TYPE)
        return (sm_object *)sym;
      sm_object *result = sm_cx_get_far(cx, sym);
      if (!result)
        return (sm_object *)sms_false;
      return result;
    }
    case SM_CX_HAS_FAR_EXPR: {
      sm_cx *cx = (sm_cx *)eager_type_check(sme, 0, SM_CX_TYPE, current_cx, sf);
      if (cx->my_type == SM_ERR_TYPE)
        return (sm_object *)cx;
      sm_symbol *sym = (sm_symbol *)eager_type_check(sme, 1, SM_SYMBOL_TYPE, current_cx, sf);
      if (sym->my_type == SM_ERR_TYPE)
        return (sm_object *)sym;
      sm_object *result = sm_cx_get_far(cx, sym);
      if (!result)
        return (sm_object *)sms_false;
      return (sm_object *)sms_true;
    }
    case SM_CX_SET_EXPR: {
      sm_cx *cx = (sm_cx *)eager_type_check(sme, 0, SM_CX_TYPE, current_cx, sf);
      if (cx->my_type == SM_ERR_TYPE)
        return (sm_object *)cx;
      sm_symbol *sym = (sm_symbol *)eager_type_check(sme, 1, SM_SYMBOL_TYPE, current_cx, sf);
      if (sym->my_type == SM_ERR_TYPE)
        return (sm_object *)sym;
      sm_object *value = (sm_object *)sm_engine_eval(sm_expr_get_arg(sme, 2), current_cx, sf);
      if (sm_cx_set(cx, sym, value))
        return (sm_object *)sms_true;
      return (sm_object *)sms_false;
    }
    case SM_CX_CLEAR_EXPR: {
      sm_cx *cx = (sm_cx *)eager_type_check(sme, 0, SM_CX_TYPE, current_cx, sf);
      if (cx->my_type != SM_CX_TYPE)
        return (sm_object *)cx;
      sm_cx_clear(cx);
      return (sm_object *)sms_true;
    }
    case SM_CX_IMPORT_EXPR: {
      sm_cx *cxFrom = (sm_cx *)eager_type_check(sme, 0, SM_CX_TYPE, current_cx, sf);
      if (cxFrom->my_type != SM_CX_TYPE)
        return (sm_object *)cxFrom;
      sm_cx *cxTo = (sm_cx *)eager_type_check(sme, 1, SM_CX_TYPE, current_cx, sf);
      if (cxTo->my_type != SM_CX_TYPE)
        return (sm_object *)cxTo;
      sm_cx_import(cxFrom, cxTo);
      return (sm_object *)sms_true;
    }
    case SM_CX_CONTAINING_EXPR: {
      sm_cx *cx = (sm_cx *)eager_type_check(sme, 0, SM_CX_TYPE, current_cx, sf);
      if (cx->my_type != SM_CX_TYPE)
        return (sm_object *)cx;
      sm_symbol *sym = (sm_symbol *)eager_type_check(sme, 1, SM_SYMBOL_TYPE, current_cx, sf);
      if (sym->my_type != SM_SYMBOL_TYPE)
        return (sm_object *)sym;
      sm_cx *retrieved = sm_cx_get_container(cx, sym);
      if (retrieved)
        return (sm_object *)retrieved;
      return (sm_object *)sms_false;
    }
    case SM_CX_SIZE_EXPR: {
      sm_cx *cx = (sm_cx *)eager_type_check(sme, 0, SM_CX_TYPE, current_cx, sf);
      if (cx->my_type != SM_CX_TYPE)
        return (sm_object *)cx;
      int size = sm_cx_size(cx);
      return (sm_object *)sm_new_double(size);
    }
    case SM_RM_EXPR: {
      sm_symbol *sym = (sm_symbol *)type_check(sme, 0, SM_SYMBOL_TYPE);

      bool success = sm_cx_rm(current_cx, sym);
      if (success == true)
        return (sm_object *)sms_true;
      return (sm_object *)sms_false;
    }
    case SM_CX_RM_EXPR: {
      sm_cx *cx = (sm_cx *)eager_type_check(sme, 0, SM_CX_TYPE, current_cx, sf);
      if (cx->my_type != SM_CX_TYPE)
        return (sm_object *)cx;
      sm_symbol *sym = (sm_symbol *)eager_type_check(sme, 1, SM_SYMBOL_TYPE, current_cx, sf);
      if (sym->my_type != SM_SYMBOL_TYPE)
        return (sm_object *)sym;
      bool success = sm_cx_rm(cx, sym);
      if (success == true)
        return (sm_object *)sms_true;
      return (sm_object *)sms_false;
    }
    case SM_CX_KEYS_EXPR: {
      sm_cx *cx = (sm_cx *)eager_type_check(sme, 0, SM_CX_TYPE, current_cx, sf);
      if (cx->my_type != SM_CX_TYPE)
        return (sm_object *)cx;
      sm_expr *success =
        sm_node_keys(cx->content, sm_new_stack_obj(32), sm_new_expr_n(SM_ARRAY_EXPR, 0, 0, NULL));
      if (success)
        return (sm_object *)success;
      return (sm_object *)sms_false;
    }
    case SM_CX_VALUES_EXPR: {
      sm_cx *cx = (sm_cx *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      if (cx->my_type != SM_CX_TYPE) {
        sm_symbol *title = sm_new_symbol("typeMismatch", 12);
        sm_string *message =
          sm_new_string(50, "Passed something that is not a context to cxValues");
        return (sm_object *)sm_new_error_from_expr(title, message, sme, NULL);
      }
      sm_expr *success = sm_node_values(cx->content, sm_new_expr_n(SM_ARRAY_EXPR, 0, 0, NULL));
      if (success)
        return (sm_object *)success;
      return (sm_object *)sms_false;
    }
    case SM_FN_XP_EXPR: {
      sm_fun *fun = (sm_fun *)eager_type_check(sme, 0, SM_FUN_TYPE, current_cx, sf);
      if (fun->my_type != SM_FUN_TYPE)
        return (sm_object *)fun;
      return sm_unlocalize((sm_object *)fun->content);
    }
    case SM_FN_SETXP_EXPR: {
      sm_fun *fun = (sm_fun *)eager_type_check(sme, 0, SM_FUN_TYPE, current_cx, sf);
      if (fun->my_type != SM_FUN_TYPE)
        return (sm_object *)fun;
      fun          = (sm_fun *)sm_copy((sm_object *)fun); // functional
      fun->content = sm_localize(sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf), fun);
      return (sm_object *)fun;
    }
    case SM_FN_PARAMS_EXPR: {
      sm_fun *fun = (sm_fun *)eager_type_check(sme, 0, SM_FUN_TYPE, current_cx, sf);
      if (fun->my_type != SM_FUN_TYPE)
        return (sm_object *)fun;
      sm_expr *result = sm_new_expr_n(SM_PARAM_LIST_EXPR, fun->num_params, fun->num_params, NULL);
      for (uint32_t i = 0; i < fun->num_params; i++) {
        sm_string *fn_name = sm_fun_get_param(fun, i)->name;
        sm_expr_set_arg(result, i, (sm_object *)sm_new_symbol(&(fn_name->content), fn_name->size));
      }
      return (sm_object *)result;
    }
    case SM_FN_SETPARAMS_EXPR: {
      sm_fun *fun = (sm_fun *)eager_type_check(sme, 0, SM_FUN_TYPE, current_cx, sf);
      if (fun->my_type != SM_FUN_TYPE)
        return (sm_object *)fun;
      sm_expr *params = (sm_expr *)eager_type_check(sme, 1, SM_EXPR_TYPE, current_cx, sf);
      if (params->my_type != SM_EXPR_TYPE)
        return (sm_object *)params;
      // Make a new function with the right size (params are part of a function)
      sm_fun *new_fun = sm_new_fun(fun->parent, params->size, fun->content);
      // Checking the parameters
      for (uint32_t i = 0; i < params->size; i++) {
        sm_symbol *sym = (sm_symbol *)sm_expr_get_arg(params, i);
        if (sym->my_type != SM_SYMBOL_TYPE) {
          sm_symbol *title = sm_new_symbol("typeMismatch", 12);
          sm_string *message =
            sm_new_fstring_at(sms_heap,
                              "When using fnSetParams(<fn>,<params>), params must be an array of "
                              "symbols. Parameter %i is not a symbol.",
                              i);
          return (sm_object *)sm_new_error_from_expr(title, message, sme, NULL);
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
      return (sm_object *)new_fun;
    } break;
    case SM_FN_PARENT_EXPR: {
      sm_fun *fun = (sm_fun *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      if (!expect_type((sm_object *)fun, SM_FUN_TYPE))
        return (sm_object *)sms_false;
      if (fun->parent)
        return (sm_object *)fun->parent;
      else
        return (sm_object *)sms_false;
    }
    case SM_FN_SETPARENT_EXPR: {
      sm_fun *fun        = (sm_fun *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_cx  *new_parent = (sm_cx *)sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      if (!expect_type((sm_object *)fun, SM_FUN_TYPE))
        return (sm_object *)sms_false;
      if (!expect_type((sm_object *)new_parent, SM_CX_TYPE))
        return (sm_object *)sms_false;
      fun         = (sm_fun *)sm_copy((sm_object *)fun);
      fun->parent = new_parent;
      return (sm_object *)fun;
    }
    case SM_XP_OP_EXPR: {
      sm_object *obj0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      if (!expect_type(obj0, SM_EXPR_TYPE))
        return (sm_object *)sms_false;
      sm_expr *expression = (sm_expr *)obj0;
      return (sm_object *)sm_new_double(expression->op);
    }
    case SM_XP_SET_OP_EXPR: {
      sm_object *obj0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      if (!expect_type(obj0, SM_EXPR_TYPE))
        return (sm_object *)sms_false;
      sm_object *obj1 = sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      if (!expect_type(obj1, SM_DOUBLE_TYPE))
        return (sm_object *)sms_false;
      sm_expr *expression = (sm_expr *)obj0;
      expression          = (sm_expr *)sm_copy((sm_object *)expression);
      sm_double *given_op = (sm_double *)obj1;
      expression->op      = (int)((sm_double *)given_op)->value;
      return (sm_object *)expression;
    }
    case SM_XP_OP_SYM_EXPR: {
      sm_object *obj0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      if (!expect_type(obj0, SM_EXPR_TYPE))
        return (sm_object *)sms_false;
      int op_num = ((sm_expr *)obj0)->op;
      return (sm_object *)sm_new_symbol(sm_global_fn_name(op_num), sm_global_fn_name_len(op_num));
    }
    case SM_STR_ESCAPE_EXPR: {
      sm_object *obj0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_string *str0;
      if (!expect_type(obj0, SM_STRING_TYPE))
        return (sm_object *)sms_false;
      str0 = (sm_string *)obj0;
      return (sm_object *)sm_string_escape(str0);
    }
    case SM_INPUT_EXPR: {
      if (sm_global_environment(NULL)->plain_mode) {
        char input_str[500];
        fgets(input_str, sizeof(input_str), stdin);
        // we remove the trailing newline character
        int len            = strlen(input_str);
        input_str[len - 1] = '\0';
        return (sm_object *)sm_new_string(len - 1, input_str);
      }
      char *line = linenoise("");
      return (sm_object *)sm_new_string(strlen(line), line);
    }
    case SM_ARGS_EXPR: {
      if (sf)
        return (sm_object *)sf;
      return (sm_object *)sms_false;
    }
    case SM_OR_EXPR: {
      sm_object *obj0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      if (!IS_FALSE(obj0)) {
        return (sm_object *)sms_true;
      }
      sm_object *obj1 = sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      if (!IS_FALSE(obj1)) {
        return (sm_object *)sms_true;
      }
      return (sm_object *)sms_false;
    }
    case SM_AND_EXPR: {
      sm_object *obj0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      if (IS_FALSE(obj0))
        return (sm_object *)sms_false;
      sm_object *obj1 = sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      if (IS_FALSE(obj1))
        return (sm_object *)sms_false;
      return (sm_object *)sms_true;
    }
    case SM_NOT_EXPR: {
      sm_object *obj0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      if (!IS_FALSE(obj0))
        return (sm_object *)sms_false;
      return (sm_object *)sms_true;
    }
    case SM_ROUND_EXPR: {
      sm_object *obj0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_double *number;
      if (expect_type(obj0, SM_DOUBLE_TYPE))
        number = (sm_double *)obj0;
      else
        return (sm_object *)sm_new_string(0, "");
      double val       = number->value;
      int    floor_val = val > 0 ? val + 0.5 : val - 0.5;
      return (sm_object *)sm_new_double(floor_val);
    }
    case SM_FLOOR_EXPR: {
      sm_object *obj0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_double *number;
      if (expect_type(obj0, SM_DOUBLE_TYPE))
        number = (sm_double *)obj0;
      else
        return (sm_object *)sm_new_string(0, "");
      double val = number->value;
      return (sm_object *)sm_new_double(floor(val));
    }
    case SM_CEIL_EXPR: {
      sm_object *obj0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_double *number;
      if (expect_type(obj0, SM_DOUBLE_TYPE))
        number = (sm_double *)obj0;
      else
        return (sm_object *)sm_new_string(0, "");
      double val = number->value;
      return (sm_object *)sm_new_double(ceil(val));
    }
    case SM_MOD_EXPR: {
      sm_object *obj0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_object *obj1 = sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      if (!expect_type(obj0, SM_DOUBLE_TYPE))
        return (sm_object *)sms_false;
      sm_double *num0 = (sm_double *)obj0;
      if (!expect_type(obj1, SM_DOUBLE_TYPE))
        return (sm_object *)sms_false;
      sm_double *num1 = (sm_double *)obj1;
      return (sm_object *)sm_new_double(fmod(num0->value, num1->value));
    }
    case SM_RANDOM_EXPR: {
      return (sm_object *)sm_new_double(((double)rand()) / ((double)RAND_MAX));
    }
    case SM_SEED_EXPR: {
      sm_object *obj0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_double *number;
      if (!expect_type(obj0, SM_DOUBLE_TYPE))
        return (sm_object *)sms_false;
      number           = (sm_double *)obj0;
      double val       = number->value;
      int    floor_val = val > 0 ? val + 0.5 : val - 0.5;
      srand((int)floor_val);
      return (sm_object *)sms_true;
    }
    case SM_FILE_WRITE_EXPR: {
      // obtain the file name
      sm_string *fname_str;
      sm_object *evaluated_fname = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      if (!expect_type(evaluated_fname, SM_STRING_TYPE))
        return (sm_object *)sms_false;
      fname_str        = (sm_string *)evaluated_fname;
      char *fname_cstr = &(fname_str->content);
      // obtain the content to write
      sm_object *evaluated_content = sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      sm_string *content_str;
      if (!expect_type(evaluated_content, SM_STRING_TYPE))
        return (sm_object *)sms_false;
      content_str        = (sm_string *)evaluated_content;
      char *content_cstr = &(content_str->content);
      FILE *fptr         = fopen(fname_cstr, "wb");
      // check that file can be opened for writing
      if (fptr == NULL) {
        printf("fileWrite failed to open: %s\n", fname_cstr);
        return (sm_object *)sms_false;
      }
      const int CHUNK_SIZE = 128;
      fwrite(content_cstr, CHUNK_SIZE, content_str->size / CHUNK_SIZE, fptr);
      if (fmod(content_str->size, CHUNK_SIZE)) {
        uint32_t cursor = (content_str->size / CHUNK_SIZE) * CHUNK_SIZE;
        fwrite(&content_cstr[cursor], 1, content_str->size - cursor, fptr);
      }

      fclose(fptr);
      return (sm_object *)sms_true;
    }
    case SM_FILE_APPEND_EXPR: {
      // obtain the file name
      sm_string *fname_str;
      sm_object *evaluated_fname = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      if (!expect_type(evaluated_fname, SM_STRING_TYPE))
        return (sm_object *)sms_false;
      fname_str        = (sm_string *)evaluated_fname;
      char *fname_cstr = &(fname_str->content);
      // obtain the content to write
      sm_object *evaluated_content = sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      sm_string *content_str;
      if (!expect_type(evaluated_content, SM_STRING_TYPE))
        return (sm_object *)sms_false;
      content_str        = (sm_string *)evaluated_content;
      char *content_cstr = &(content_str->content);
      FILE *fptr         = fopen(fname_cstr, "a");
      // check that file can be opened for writing
      if (fptr == NULL) {
        printf("fileAppendStr failed to open for appending: %s\n", fname_cstr);
        return (sm_object *)sms_false;
      }
      fputs(content_cstr, fptr);
      fclose(fptr);
      return (sm_object *)sms_true;
    }
    case SM_FILE_READ_EXPR: {
      sm_object *evaluated = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_string *fname_str;
      if (!expect_type(evaluated, SM_STRING_TYPE))
        return (sm_object *)sms_false;
      fname_str        = (sm_string *)evaluated;
      char *fname_cstr = &(fname_str->content);
      if (access(fname_cstr, F_OK) != 0) {
        printf("fileReadStr failed because the file, %s ,does not exist.\n", fname_cstr);
        return (sm_object *)sm_new_string(0, "");
      }
      FILE *fptr = fopen(fname_cstr, "r");
      fseek(fptr, 0, SEEK_END);
      long       len    = ftell(fptr);
      sm_string *output = sm_new_string_manual(len);
      fseek(fptr, 0, SEEK_SET);
      fread(&(output->content), 1, len, fptr);
      fclose(fptr);
      *(&output->content + len) = '\0';
      return (sm_object *)output;
    }
    case SM_FILE_PART_EXPR: {
      sm_object *evaluated = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_string *fname_str;
      if (!expect_type(evaluated, SM_STRING_TYPE))
        return (sm_object *)sms_false;
      fname_str        = (sm_string *)evaluated;
      char *fname_cstr = &(fname_str->content);
      evaluated        = sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      sm_double *start_pos;
      if (!expect_type(evaluated, SM_DOUBLE_TYPE))
        return (sm_object *)sms_false;
      start_pos = (sm_double *)evaluated;
      evaluated = sm_engine_eval(sm_expr_get_arg(sme, 2), current_cx, sf);
      sm_double *length;
      if (!expect_type(evaluated, SM_DOUBLE_TYPE))
        return (sm_object *)sms_false;
      length = (sm_double *)evaluated;
      if (access(fname_cstr, F_OK) != 0) {
        printf("filePart failed because the file, %s ,does not exist.\n", fname_cstr);
        return (sm_object *)sm_new_string(0, "");
      }
      FILE *fptr = fopen(fname_cstr, "r");
      // Get length of file
      fseek(fptr, 0, SEEK_END);
      long file_length = ftell(fptr);

      if (file_length < length->value) {
        fclose(fptr);
        printf("filePart failed because part length is out of range: %i\n", (int)length->value);
      }
      fseek(fptr, start_pos->value, SEEK_SET);
      sm_string *output = sm_new_string_manual((int)length->value);
      fread(&(output->content), 1, (int)length->value, fptr);
      fclose(fptr);
      *(&output->content + ((int)length->value)) = '\0';
      return (sm_object *)output;
    }
    case SM_FILE_EXISTS_EXPR: {
      sm_object *evaluated = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_string *fname_str;
      if (!expect_type(evaluated, SM_STRING_TYPE))
        return (sm_object *)sms_false;
      fname_str        = (sm_string *)evaluated;
      char *fname_cstr = &(fname_str->content);

      FILE *file = fopen(fname_cstr, "r");
      if (file == NULL) {
        return (sm_object *)sms_false;
      }
      fclose(file);
      return (sm_object *)sms_true;
    }
    case SM_FILE_RM_EXPR: {
      sm_object *evaluated = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_string *fname_str;
      if (!expect_type(evaluated, SM_STRING_TYPE))
        return (sm_object *)sms_false;
      fname_str        = (sm_string *)evaluated;
      char *fname_cstr = &(fname_str->content);

      int result = remove(fname_cstr);
      if (result != 0) {
        printf("fileRm failed: Could not rm file: %s\n", fname_cstr);
        return (sm_object *)sms_false;
      }
      return (sm_object *)sms_true;
    }
    case SM_FILE_STAT_EXPR: {
      sm_object *evaluated = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_string *fname_str;
      if (!expect_type(evaluated, SM_STRING_TYPE))
        return (sm_object *)sms_false;
      fname_str              = (sm_string *)evaluated;
      char       *fname_cstr = &(fname_str->content);
      struct stat filestat;
      sm_expr    *output;
      if (stat(fname_cstr, &filestat) == 0) {
        // build an array with stat information.
        output = sm_new_expr_n(SM_ARRAY_EXPR, 16, 16, NULL);
        sm_expr_set_arg(output, 0, (sm_object *)sm_new_double(filestat.st_dev));
        sm_expr_set_arg(output, 1, (sm_object *)sm_new_double(filestat.st_ino));
        sm_expr_set_arg(output, 2, (sm_object *)sm_new_double(filestat.st_mode));
        sm_expr_set_arg(output, 3, (sm_object *)sm_new_double(filestat.st_nlink));
        sm_expr_set_arg(output, 4, (sm_object *)sm_new_double(filestat.st_uid));
        sm_expr_set_arg(output, 5, (sm_object *)sm_new_double(filestat.st_gid));
        sm_expr_set_arg(output, 6, (sm_object *)sm_new_double(filestat.st_rdev));
        sm_expr_set_arg(output, 7, (sm_object *)sm_new_double(filestat.st_size));
        sm_expr_set_arg(output, 8, (sm_object *)sm_new_double(filestat.st_blksize));
        sm_expr_set_arg(output, 9, (sm_object *)sm_new_double(filestat.st_blocks));
        sm_expr_set_arg(output, 12, (sm_object *)sm_new_double(filestat.st_mtime));
        sm_expr_set_arg(output, 14, (sm_object *)sm_new_double(filestat.st_ctime));
#if __APPLE__
        sm_expr_set_arg(output, 10, (sm_object *)sm_new_double(filestat.st_atime));
        sm_expr_set_arg(output, 11, (sm_object *)sm_new_double(0));
        sm_expr_set_arg(output, 13, (sm_object *)sm_new_double(0));
        sm_expr_set_arg(output, 15, (sm_object *)sm_new_double(0));
#else
        sm_expr_set_arg(output, 10, (sm_object *)sm_new_double(filestat.st_atim.tv_sec));
        sm_expr_set_arg(output, 11, (sm_object *)sm_new_double(filestat.st_atim.tv_nsec));
        sm_expr_set_arg(output, 13, (sm_object *)sm_new_double(filestat.st_mtim.tv_nsec));
        sm_expr_set_arg(output, 15, (sm_object *)sm_new_double(filestat.st_ctim.tv_nsec));
#endif
      } else {
        printf("Failed to get file information.\n");
        return (sm_object *)sms_false;
      }
      return (sm_object *)output;
    }
    case SM_FILE_PARSE_EXPR: {
      sm_object *evaluated = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_string *str;
      if (!expect_type(evaluated, SM_STRING_TYPE))
        return (sm_object *)sms_false;
      str                  = (sm_string *)evaluated;
      char           *cstr = &(str->content);
      sm_parse_result pr   = sm_parse_file(cstr);
      if (pr.return_val != 0) {
        printf("Error: Parser failed and returned %i.\n", pr.return_val);
        return (sm_object *)sms_false;
      }
      return pr.parsed_object;
      break;
    }
    case SM_PARSE_EXPR: {
      sm_object *evaluated = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_string *str;
      if (!expect_type(evaluated, SM_STRING_TYPE))
        return (sm_object *)sms_false;
      str                = (sm_string *)evaluated;
      char *cstr         = &(str->content);
      cstr[str->size]    = ';'; // Temporarily replacing the NULL char
      sm_parse_result pr = sm_parse_cstr(cstr, str->size + 1);
      cstr[str->size]    = '\0'; // Place the null char back
      if (pr.return_val != 0) {
        printf("Error: Parser failed and returned %i.\n", pr.return_val);
        return (sm_object *)sms_false;
      }
      return pr.parsed_object;
    }
    case SM_NEW_STR_EXPR: {
      sm_object *evaluated = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      return (sm_object *)sm_object_to_string(evaluated);
    }
    case SM_EVAL_EXPR: {
      sm_object *evaluated = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      return sm_engine_eval(evaluated, current_cx, sf);
    }
    case SM_CX_EVAL_EXPR: {
      sm_object *evaluated = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_object *obj1      = sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      sm_cx     *where_to_eval;
      if (!expect_type(evaluated, SM_CX_TYPE))
        return (sm_object *)sms_true;
      where_to_eval = (sm_cx *)evaluated;
      return sm_engine_eval(obj1, where_to_eval, sf);
    }
    case SM_PUT_EXPR: {
      sm_string *str;
      for (int i = 0; i < sme->size; i++) {
        sm_object *evaluated = sm_engine_eval(sm_expr_get_arg(sme, i), current_cx, sf);
        if (!expect_type(evaluated, SM_STRING_TYPE)) {
          sm_symbol *title   = sm_new_symbol("typeMismatch", 12);
          sm_string *message = sm_new_fstring_at(
            sms_heap, "put function takes strings, but parameter %i was not a string", i);
          return (sm_object *)sm_new_error_from_expr(title, message, sme, NULL);
        }
        str = (sm_string *)evaluated;
        for (uint32_t i = 0; i < str->size; i++)
          putchar((&str->content)[i]);
        putchar('\0');
      }
      fflush(stdout);
      return (sm_object *)sms_true;
      break;
    }
    case SM_PUTLN_EXPR: {
      sm_string *str;
      sm_object *evaluated;
      for (int i = 0; i < sme->size; i++) {
        evaluated = sm_engine_eval(sm_expr_get_arg(sme, i), current_cx, sf);
        if (!expect_type(evaluated, SM_STRING_TYPE)) {
          sm_symbol *title   = sm_new_symbol("typeMismatch", 12);
          sm_string *message = sm_new_fstring_at(
            sms_heap, "putLn function takes strings, but parameter %i was not a string", i);
          return (sm_object *)sm_new_error_from_expr(title, message, sme, NULL);
        }
        str = (sm_string *)evaluated;
        for (uint32_t i = 0; i < str->size; i++)
          putchar((&str->content)[i]);
        putchar('\n');
        putchar('\0');
      }
      fflush(stdout);
      return (sm_object *)sms_true;
      break;
    }
    case SM_WHILE_EXPR: {
      sm_expr   *condition  = (sm_expr *)sm_expr_get_arg(sme, 0);
      sm_object *expression = sm_expr_get_arg(sme, 1);
      sm_object *result     = (sm_object *)sms_true;
      while (!IS_FALSE(sm_engine_eval((sm_object *)condition, current_cx, sf))) {
        result = sm_engine_eval(expression, current_cx, sf);
        if (result->my_type == SM_RETURN_TYPE)
          return result;
      }
      return result;
      break;
    }
    case SM_FOR_EXPR: {
      sm_expr   *init       = (sm_expr *)sm_expr_get_arg(sme, 0);
      sm_expr   *condition  = (sm_expr *)sm_expr_get_arg(sme, 1);
      sm_expr   *increment  = (sm_expr *)sm_expr_get_arg(sme, 2);
      sm_object *expression = sm_expr_get_arg(sme, 3);
      sm_cx     *inner_cx   = sm_new_cx(current_cx);
      // Run init 1 time
      sm_engine_eval((sm_object *)init, inner_cx, sf);
      sm_object *result = (sm_object *)sms_false;
      // If it's a block, copy the block to set the inner_cx as scope
      if (expression->my_type == SM_EXPR_TYPE && ((sm_expr *)expression)->op == SM_BLOCK_EXPR) {
        expression = sm_copy(expression);
        sm_expr_set_arg((sm_expr *)expression, 0, (sm_object *)inner_cx);
      }
      while (!IS_FALSE(sm_engine_eval((sm_object *)condition, inner_cx, sf))) {
        result = sm_engine_eval(expression, inner_cx, sf);
        if (result->my_type == SM_RETURN_TYPE)
          return result;
        // Run increment after each loop
        sm_engine_eval((sm_object *)increment, inner_cx, sf);
      }
      return result;
      break;
    }
    case SM_DO_WHILE_EXPR: {
      sm_expr   *condition  = (sm_expr *)sm_expr_get_arg(sme, 1);
      sm_object *expression = sm_expr_get_arg(sme, 0);
      sm_object *result     = (sm_object *)sms_true;
      do {
        result = sm_engine_eval(expression, current_cx, sf);
        if (result->my_type == SM_RETURN_TYPE)
          return result;
      } while (!IS_FALSE(sm_engine_eval((sm_object *)condition, current_cx, sf)));
      return result;
      break;
    }
    case SM_RETURN_EXPR: {
      sm_object *to_return = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      return (sm_object *)sm_new_return(to_return);
    }
    case SM_SIZE_EXPR: {
      sm_object *base_obj = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_expr   *arr;
      if (!expect_type(base_obj, SM_EXPR_TYPE))
        return (sm_object *)sms_false;
      arr = (sm_expr *)base_obj;
      return (sm_object *)sm_new_double(arr->size);
    }
    case SM_MAP_EXPR: {
      // expecting a unary func
      sm_object *obj0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_fun    *fun;
      if (!expect_type(obj0, SM_FUN_TYPE))
        return (sm_object *)sms_false;
      fun             = (sm_fun *)obj0;
      sm_object *obj1 = sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      sm_expr   *arr;
      if (!expect_type(obj1, SM_EXPR_TYPE))
        return (sm_object *)sms_false;
      arr             = (sm_expr *)obj1;
      sm_expr *output = sm_new_expr_n(arr->op, arr->size, arr->size, NULL);
      for (uint32_t i = 0; i < arr->size; i++) {
        sm_object *current_obj = sm_expr_get_arg(arr, i);
        sm_expr   *new_sf      = sm_new_expr(SM_PARAM_LIST_EXPR, current_obj, NULL);

        sm_object *map_result = sm_engine_eval(fun->content, fun->parent, new_sf);

        if (map_result->my_type == SM_RETURN_TYPE)
          map_result = ((sm_return *)map_result)->address;

        sm_expr_set_arg(output, i, map_result);
      }
      return (sm_object *)output;
    }
    case SM_REDUCE_EXPR: {
      // expecting a binary function
      sm_object *obj0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_fun    *fun;
      if (!expect_type(obj0, SM_FUN_TYPE))
        return (sm_object *)sms_false;
      fun = (sm_fun *)obj0;
      // evaluating the expression to reduce
      sm_object *obj1 = sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      sm_expr   *arr;
      if (!expect_type(obj1, SM_EXPR_TYPE))
        return (sm_object *)sms_false;
      arr = (sm_expr *)obj1;
      // initial value for reduction
      sm_object *initial = sm_engine_eval(sm_expr_get_arg(sme, 2), current_cx, sf);
      // reducing the expression
      sm_object *result = initial;
      // Evaluating with a reusable stack frame: ( result , current_ob )
      sm_expr *reusable = sm_new_expr_2(SM_PARAM_LIST_EXPR, result, NULL, NULL);
      for (uint32_t i = 0; i < arr->size; i++) {
        sm_object *current_obj = sm_expr_get_arg(arr, i);
        sm_expr_set_arg(reusable, 1, current_obj);
        result = sm_engine_eval(fun->content, fun->parent, reusable);
        sm_expr_set_arg(reusable, 0, result);
      }
      return result;
    }
    case SM_INDEX_EXPR: {
      sm_object *base_obj = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_expr   *arr;
      if (!expect_type(base_obj, SM_EXPR_TYPE)) {
        sm_symbol *title = sm_new_symbol("typeMismatch", 12);
        sm_string *message =
          sm_new_string(59, "Trying to apply index op to something that is not an array.");
        return (sm_object *)sm_new_error_from_expr(title, message, sme, NULL);
      }
      arr                  = (sm_expr *)base_obj;
      sm_object *index_obj = sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      sm_double *index_double;
      if (!expect_type(index_obj, SM_DOUBLE_TYPE))
        return (sm_object *)sm_new_string(0, "");
      index_double   = (sm_double *)index_obj;
      uint32_t index = (int)index_double->value;
      if (arr->size < index + 1 || index_double->value < 0) {
        sm_string *message = sm_new_fstring_at(
          sms_heap, "Index out of range: %i . Array size is %i", index, arr->size);
        sm_symbol *title = sm_new_symbol("arrayIndexOutOfBounds", 21);
        return (sm_object *)sm_new_error_from_expr(title, message, sme, NULL);
      }
      return sm_expr_get_arg(arr, index);
    }
    case SM_DOT_EXPR: {
      sm_object *base_obj   = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_symbol *field_sym  = (sm_symbol *)sm_expr_get_arg(sme, 1);
      sm_string *field_name = field_sym->name;
      sm_cx     *base_cx;
      if (base_obj->my_type != SM_CX_TYPE) {
        sm_symbol *title   = sm_new_symbol("notACx", 6);
        sm_string *message = sm_new_fstring_at(sms_heap, "Attempted x.%s where x was not a contex",
                                               &field_name->content);
      }
      base_cx       = (sm_cx *)base_obj;
      sm_object *sr = sm_cx_get_far(base_cx, field_sym);
      if (sr == NULL) {
        sm_symbol *title   = sm_new_symbol("varNotFound", 11);
        sm_string *cx_str  = sm_object_to_string((sm_object *)current_cx);
        sm_string *message = sm_new_fstring_at(sms_heap, "variable: %s not found in cx: %s",
                                               &field_name->content, &cx_str->content);
        return (sm_object *)sm_new_error_from_expr(title, message, sme, NULL);
      }
      return (sm_object *)sr;
    }
    case SM_PARENT_EXPR: {
      sm_object *base_obj = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_cx     *base_cx  = (sm_cx *)eager_type_check(sme, 0, SM_CX_TYPE, current_cx, sf);
      if (base_cx->parent == NULL)
        return (sm_object *)sms_false;
      return (sm_object *)base_cx->parent;
    }
    case SM_DIFF_EXPR: {
      sm_object *evaluated0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_object *evaluated1 = sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      sm_symbol *sym1;
      if (!expect_type(evaluated1, SM_SYMBOL_TYPE))
        return (sm_object *)sms_false;
      sym1              = (sm_symbol *)evaluated1;
      sm_object *result = sm_diff(evaluated0, sym1);
      return sm_simplify(result);
    }
    case SM_SIMP_EXPR: {
      sm_object *evaluated0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      return sm_simplify(evaluated0);
    }
    case SM_FUN_CALL_EXPR: {
      sm_object      *result;
      struct sm_expr *args     = (struct sm_expr *)sm_expr_get_arg(sme, 1);
      sm_expr        *new_args = (sm_expr *)sm_engine_eval((sm_object *)args, current_cx, sf);
      sm_object      *obj0     = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      if (obj0->my_type == SM_FUN_TYPE) {
        sm_fun    *fun     = (sm_fun *)obj0;
        sm_object *content = fun->content;
        if (content->my_type == SM_EXPR_TYPE && ((sm_expr *)content)->op == SM_BLOCK_EXPR) {
          sm_cx   *new_cx      = sm_new_cx(fun->parent);
          uint32_t i           = 1;
          sm_expr *content_sme = (sm_expr *)fun->content;
          while (i < content_sme->size) {
            result = sm_engine_eval(sm_expr_get_arg(content_sme, i), new_cx, new_args);
            if (result->my_type == SM_RETURN_TYPE)
              break;
            i++;
          }
        } else
          result = sm_engine_eval(fun->content, fun->parent, new_args);
        if (result->my_type == SM_RETURN_TYPE)
          return ((sm_return *)result)->address;
        else
          return result;
      } else
        return obj0;
      break;
    }
    case SM_BLOCK_EXPR: {
      uint32_t   i      = 1;
      sm_object *result = (sm_object *)sms_true;
      sm_cx     *new_cx = sm_new_cx(current_cx);
      while (i < sme->size) {
        result = sm_engine_eval(sm_expr_get_arg(sme, i), new_cx, sf);
        if (result->my_type == SM_RETURN_TYPE)
          return result;
        i++;
      }
      return result;
      break;
    }
    case SM_ASSIGN_EXPR: {
      sm_symbol *sym;
      sm_object *obj0  = sm_expr_get_arg(sme, 0);
      sm_object *value = (sm_object *)sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      if (!expect_type(obj0, SM_SYMBOL_TYPE))
        return (sm_object *)sms_false;
      sym = (sm_symbol *)obj0;
      if (!sm_cx_set(current_cx, sym, value))
        return (sm_object *)sms_false;
      return value;
    }
    /*case SM_ASSIGN_DOT_EXPR: {
     * bison parser doesn't like this
      if (!sm_cx_let(predot, postdot, value))
        return (sm_object *)sms_false;
      return (sm_object *)sms_true;
    }*/
    case SM_ASSIGN_LOCAL_EXPR: {
      sm_object *obj0  = sm_expr_get_arg(sme, 0);
      sm_object *value = (sm_object *)sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      if (obj0->my_type == SM_LOCAL_TYPE) {
        sm_local *lcl = (sm_local *)obj0;
        sm_expr_set_arg(sf, lcl->index, value);
      } else
        return (sm_object *)sms_false;
      return value;
      break;
    }
    case SM_ASSIGN_INDEX_EXPR: {
      // expecting a[4]=value=> =index_expr(a,4,value);
      sm_object *value = sm_engine_eval(sm_expr_get_arg(sme, 2), current_cx, sf);
      sm_expr   *arr   = (sm_expr *)eager_type_check(sme, 0, SM_EXPR_TYPE, current_cx, sf);
      sm_double *index = (sm_double *)eager_type_check(sme, 1, SM_DOUBLE_TYPE, current_cx, sf);
      if (index->value >= arr->size || index->value < -0) {
        printf("Error: Index out of range: %i\n", (int)index->value);
        sm_symbol *title = sm_new_symbol("indexOutOfBounds", 16);
        sm_string *message =
          sm_new_fstring_at(sms_heap, "Index %i is out of bounds of array of size %i",
                            (int)index->value, (int)arr->size);
        return (sm_object *)sm_new_error_from_expr(title, message, sme, NULL);
      }
      sm_expr_set_arg(arr, index->value, value);
      return (sm_object *)sms_true;
      break;
    }
    case SM_PLUS_EXPR: {
      sm_double *obj0 = (sm_double *)eager_type_check(sme, 0, SM_DOUBLE_TYPE, current_cx, sf);
      if (obj0->my_type != SM_DOUBLE_TYPE)
        return (sm_object *)obj0;
      sm_double *obj1 = (sm_double *)eager_type_check(sme, 1, SM_DOUBLE_TYPE, current_cx, sf);
      if (obj1->my_type != SM_DOUBLE_TYPE)
        return (sm_object *)obj1;
      return (sm_object *)sm_new_double(obj0->value + obj1->value);
      break;
    }
    case SM_MINUS_EXPR: {
      sm_double *obj0 = (sm_double *)eager_type_check(sme, 0, SM_DOUBLE_TYPE, current_cx, sf);
      if (obj0->my_type != SM_DOUBLE_TYPE)
        return (sm_object *)obj0;
      sm_double *obj1 = (sm_double *)eager_type_check(sme, 1, SM_DOUBLE_TYPE, current_cx, sf);
      if (obj1->my_type != SM_DOUBLE_TYPE)
        return (sm_object *)obj1;
      return (sm_object *)sm_new_double(obj0->value - obj1->value);
      break;
    }
    case SM_TIMES_EXPR: {
      sm_double *obj0 = (sm_double *)eager_type_check(sme, 0, SM_DOUBLE_TYPE, current_cx, sf);
      if (obj0->my_type == SM_ERR_TYPE)
        return (sm_object *)obj0;
      sm_double *obj1 = (sm_double *)eager_type_check(sme, 1, SM_DOUBLE_TYPE, current_cx, sf);
      if (obj1->my_type == SM_ERR_TYPE)
        return (sm_object *)obj1;
      return (sm_object *)sm_new_double(obj0->value * obj1->value);
      break;
    }
    case SM_DIVIDE_EXPR: {
      sm_double *obj0 = (sm_double *)eager_type_check(sme, 0, SM_DOUBLE_TYPE, current_cx, sf);
      if (obj0->my_type == SM_ERR_TYPE)
        return (sm_object *)obj0;
      sm_double *obj1 = (sm_double *)eager_type_check(sme, 1, SM_DOUBLE_TYPE, current_cx, sf);
      if (obj1->my_type == SM_ERR_TYPE)
        return (sm_object *)obj1;
      return (sm_object *)sm_new_double(obj0->value / obj1->value);
      break;
    }
    case SM_POW_EXPR: {
      sm_double *obj0 = (sm_double *)eager_type_check(sme, 0, SM_DOUBLE_TYPE, current_cx, sf);
      if (obj0->my_type == SM_ERR_TYPE)
        return (sm_object *)obj0;
      sm_double *obj1 = (sm_double *)eager_type_check(sme, 1, SM_DOUBLE_TYPE, current_cx, sf);
      if (obj1->my_type == SM_ERR_TYPE)
        return (sm_object *)obj1;
      return (sm_object *)sm_new_double(pow(obj0->value, obj1->value));
      break;
    }
    case SM_SIN_EXPR: {
      sm_double *num0 = (sm_double *)eager_type_check(sme, 0, SM_DOUBLE_TYPE, current_cx, sf);
      if (num0->my_type == SM_ERR_TYPE)
        return (sm_object *)num0;
      return (sm_object *)sm_new_double(sin(num0->value));
      break;
    }
    case SM_COS_EXPR: {
      sm_double *num0 = (sm_double *)eager_type_check(sme, 0, SM_DOUBLE_TYPE, current_cx, sf);
      if (num0->my_type == SM_ERR_TYPE)
        return (sm_object *)num0;
      return (sm_object *)sm_new_double(cos(num0->value));
      break;
    }
    case SM_TAN_EXPR: {
      sm_double *num0 = (sm_double *)eager_type_check(sme, 0, SM_DOUBLE_TYPE, current_cx, sf);
      if (num0->my_type == SM_ERR_TYPE)
        return (sm_object *)num0;
      return (sm_object *)sm_new_double(tan(num0->value));
      break;
    }
    case SM_ASIN_EXPR: {
      sm_double *num0 = (sm_double *)eager_type_check(sme, 0, SM_DOUBLE_TYPE, current_cx, sf);
      if (num0->my_type == SM_ERR_TYPE)
        return (sm_object *)num0;
      return (sm_object *)sm_new_double(asin(num0->value));
      break;
    }
    case SM_ACOS_EXPR: {
      sm_double *num0 = (sm_double *)eager_type_check(sme, 0, SM_DOUBLE_TYPE, current_cx, sf);
      if (num0->my_type == SM_ERR_TYPE)
        return (sm_object *)num0;
      return (sm_object *)sm_new_double(acos(num0->value));
      break;
    }

    case SM_ATAN_EXPR: {
      sm_double *num0 = (sm_double *)eager_type_check(sme, 0, SM_DOUBLE_TYPE, current_cx, sf);
      if (num0->my_type == SM_ERR_TYPE)
        return (sm_object *)num0;
      return (sm_object *)sm_new_double(atan(num0->value));
      break;
    }
    case SM_SEC_EXPR: {
      sm_double *num0 = (sm_double *)eager_type_check(sme, 0, SM_DOUBLE_TYPE, current_cx, sf);
      if (num0->my_type == SM_ERR_TYPE)
        return (sm_object *)num0;
      return (sm_object *)sm_new_double(1.0 / cos(num0->value));
      break;
    }
    case SM_CSC_EXPR: {
      sm_double *num0 = (sm_double *)eager_type_check(sme, 0, SM_DOUBLE_TYPE, current_cx, sf);
      if (num0->my_type == SM_ERR_TYPE)
        return (sm_object *)num0;
      return (sm_object *)sm_new_double(1.0 / sin(num0->value));
      break;
    }
    case SM_COT_EXPR: {
      sm_double *num0 = (sm_double *)eager_type_check(sme, 0, SM_DOUBLE_TYPE, current_cx, sf);
      if (num0->my_type == SM_ERR_TYPE)
        return (sm_object *)num0;
      return (sm_object *)sm_new_double(1.0 / tan(num0->value));
      break;
    }
    case SM_SINH_EXPR: {
      sm_double *num0 = (sm_double *)eager_type_check(sme, 0, SM_DOUBLE_TYPE, current_cx, sf);
      if (num0->my_type == SM_ERR_TYPE)
        return (sm_object *)num0;
      return (sm_object *)sm_new_double(sinh(num0->value));
      break;
    }
    case SM_COSH_EXPR: {
      sm_double *num0 = (sm_double *)eager_type_check(sme, 0, SM_DOUBLE_TYPE, current_cx, sf);
      if (num0->my_type == SM_ERR_TYPE)
        return (sm_object *)num0;
      return (sm_object *)sm_new_double(cosh(num0->value));
      break;
    }
    case SM_TANH_EXPR: {
      sm_double *num0 = (sm_double *)eager_type_check(sme, 0, SM_DOUBLE_TYPE, current_cx, sf);
      if (num0->my_type == SM_ERR_TYPE)
        return (sm_object *)num0;
      return (sm_object *)sm_new_double(tanh(num0->value));
      break;
    }
    case SM_SECH_EXPR: {
      sm_double *num0 = (sm_double *)eager_type_check(sme, 0, SM_DOUBLE_TYPE, current_cx, sf);
      if (num0->my_type == SM_ERR_TYPE)
        return (sm_object *)num0;
      return (sm_object *)sm_new_double(1.0 / cosh(num0->value));
      break;
    }
    case SM_CSCH_EXPR: {
      sm_double *num0 = (sm_double *)eager_type_check(sme, 0, SM_DOUBLE_TYPE, current_cx, sf);
      if (num0->my_type == SM_ERR_TYPE)
        return (sm_object *)num0;
      return (sm_object *)sm_new_double(1.0 / sinh(num0->value));
      break;
    }
    case SM_COTH_EXPR: {
      sm_double *num0 = (sm_double *)eager_type_check(sme, 0, SM_DOUBLE_TYPE, current_cx, sf);
      if (num0->my_type == SM_ERR_TYPE)
        return (sm_object *)num0;
      return (sm_object *)sm_new_double(1.0 / tanh(num0->value));
      break;
    }
    case SM_LN_EXPR: {
      sm_double *num0 = (sm_double *)eager_type_check(sme, 0, SM_DOUBLE_TYPE, current_cx, sf);
      if (num0->my_type == SM_ERR_TYPE)
        return (sm_object *)num0;
      return (sm_object *)sm_new_double(log(num0->value));
      break;
    }
    case SM_LOG_EXPR: {
      sm_double *num0 = (sm_double *)eager_type_check(sme, 0, SM_DOUBLE_TYPE, current_cx, sf);
      if (num0->my_type == SM_ERR_TYPE)
        return (sm_object *)num0;
      sm_double *num1 = (sm_double *)eager_type_check(sme, 1, SM_DOUBLE_TYPE, current_cx, sf);
      if (num1->my_type == SM_ERR_TYPE)
        return (sm_object *)num1;
      return (sm_object *)sm_new_double(log(num1->value) / log(num0->value));
      break;
    }
    case SM_EXP_EXPR: {
      sm_double *num0 = (sm_double *)eager_type_check(sme, 0, SM_DOUBLE_TYPE, current_cx, sf);
      if (num0->my_type == SM_ERR_TYPE)
        return (sm_object *)num0;
      return (sm_object *)sm_new_double(exp(num0->value));
      break;
    }
    case SM_SQRT_EXPR: {
      sm_double *num0 = (sm_double *)eager_type_check(sme, 0, SM_DOUBLE_TYPE, current_cx, sf);
      if (num0->my_type == SM_ERR_TYPE)
        return (sm_object *)num0;
      return (sm_object *)sm_new_double(sqrt(num0->value));
      break;
    }
    case SM_ABS_EXPR: {
      sm_double *num0 = (sm_double *)eager_type_check(sme, 0, SM_DOUBLE_TYPE, current_cx, sf);
      if (num0->my_type == SM_ERR_TYPE)
        return (sm_object *)num0;
      return (sm_object *)sm_new_double(num0->value < 0 ? -1 * num0->value : num0->value);
      break;
    }
    case SM_IF_EXPR: {
      sm_object *condition_result = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      if (!IS_FALSE(condition_result)) {
        return sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      }
      return (sm_object *)sms_false;
    }
    case SM_IF_ELSE_EXPR: {
      sm_object *condition_result = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      if (!IS_FALSE(condition_result)) {
        return sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      }
      return sm_engine_eval(sm_expr_get_arg(sme, 2), current_cx, sf);
    }
    case SM_ARRAY_EXPR: {
      sm_expr *new_arr = sm_new_expr_n(SM_ARRAY_EXPR, sme->size, sme->size, NULL);
      for (uint32_t i = 0; i < sme->size; i++) {
        sm_object *new_val = sm_engine_eval(sm_expr_get_arg(sme, i), current_cx, sf);
        sm_expr_set_arg(new_arr, i, new_val);
      }
      return (sm_object *)new_arr;
    }
    case SM_PARAM_LIST_EXPR: {
      sm_expr *new_arr = sm_new_expr_n(SM_PARAM_LIST_EXPR, sme->size, sme->size, NULL);
      for (uint32_t i = 0; i < sme->size; i++) {
        sm_object *new_val = sm_engine_eval(sm_expr_get_arg(sme, i), current_cx, sf);
        sm_expr_set_arg(new_arr, i, new_val);
      }
      return (sm_object *)new_arr;
    }
    case SM_LT_EXPR: {
      sm_double *obj1 = (sm_double *)eager_type_check(sme, 0, SM_DOUBLE_TYPE, current_cx, sf);
      if (obj1->my_type == SM_ERR_TYPE)
        return (sm_object *)obj1;
      sm_double *obj2 = (sm_double *)eager_type_check(sme, 1, SM_DOUBLE_TYPE, current_cx, sf);
      if (obj2->my_type == SM_ERR_TYPE)
        return (sm_object *)obj2;
      if (obj1->value < obj2->value) {
        return (sm_object *)sms_true;
      } else {
        return (sm_object *)sms_false;
      }
    }
    case SM_GT_EXPR: {
      sm_double *obj1 = (sm_double *)eager_type_check(sme, 0, SM_DOUBLE_TYPE, current_cx, sf);
      if (obj1->my_type == SM_ERR_TYPE)
        return (sm_object *)obj1;
      sm_double *obj2 = (sm_double *)eager_type_check(sme, 1, SM_DOUBLE_TYPE, current_cx, sf);
      if (obj2->my_type == SM_ERR_TYPE)
        return (sm_object *)obj2;
      if (obj1->value > obj2->value) {
        return (sm_object *)sms_true;
      } else {
        return (sm_object *)sms_false;
      }
    }
    case SM_EQEQ_EXPR: {
      sm_object *obj1 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_object *obj2 = sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      if (obj1 == obj2) {
        return (sm_object *)sms_true;
      }
      if (sm_object_eq(obj1, obj2))
        return (sm_object *)sms_true;
      return (sm_object *)sms_false;
    }
    case SM_IS_EXPR: {
      sm_object *obj1 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_object *obj2 = sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      if (obj1 == obj2)
        return (sm_object *)sms_true;
      return (sm_object *)sms_false;
    }
    case SM_GT_EQ_EXPR: {
      sm_double *obj0 = (sm_double *)eager_type_check(sme, 0, SM_DOUBLE_TYPE, current_cx, sf);
      if (obj0->my_type == SM_ERR_TYPE)
        return (sm_object *)obj0;

      sm_double *obj1 = (sm_double *)eager_type_check(sme, 1, SM_DOUBLE_TYPE, current_cx, sf);
      if (obj1->my_type == SM_ERR_TYPE)
        return (sm_object *)obj1;

      if (obj0->value >= obj1->value)
        return (sm_object *)sms_true;
      return (sm_object *)sms_false;
    }
    case SM_LT_EQ_EXPR: {
      sm_double *obj0 = (sm_double *)eager_type_check(sme, 0, SM_DOUBLE_TYPE, current_cx, sf);
      if (obj0->my_type == SM_ERR_TYPE)
        return (sm_object *)obj0;

      sm_double *obj1 = (sm_double *)eager_type_check(sme, 1, SM_DOUBLE_TYPE, current_cx, sf);
      if (obj1->my_type == SM_ERR_TYPE)
        return (sm_object *)obj1;

      if (obj0->value <= obj1->value)
        return (sm_object *)sms_true;
      return (sm_object *)sms_false;
    }
    case SM_ISNAN_EXPR: {
      sm_double *num0 = (sm_double *)eager_type_check(sme, 0, SM_DOUBLE_TYPE, current_cx, sf);
      if (num0->my_type == SM_ERR_TYPE)
        return (sm_object *)num0;
      if (isnan(num0->value))
        return (sm_object *)sms_true;
      else
        return (sm_object *)sms_false;
      break;
    }
    case SM_ISINF_EXPR: {
      sm_double *num0 = (sm_double *)eager_type_check(sme, 0, SM_DOUBLE_TYPE, current_cx, sf);
      if (num0->my_type == SM_ERR_TYPE)
        return (sm_object *)num0;
      if (isinf(num0->value))
        return (sm_object *)sms_true;
      else
        return (sm_object *)sms_false;
      break;
    }
    case SM_RUNTIME_META_EXPR: {
      sm_object *obj = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      return (sm_object *)sm_new_meta(obj, current_cx);
    }
    case SM_ISERR_EXPR: {
      sm_object *obj0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      if (obj0->my_type == SM_ERR_TYPE)
        return (sm_object *)sms_true;
      return (sm_object *)sms_false;
    }
    case SM_ERRTITLE_EXPR: {
      sm_error *obj0 = (sm_error *)eager_type_check(sme, 0, SM_ERR_TYPE, current_cx, sf);
      sm_error *e    = (sm_error *)obj0;
      return (sm_object *)e->title;
    }
    case SM_ERRLINE_EXPR: {
      sm_error *obj0 = (sm_error *)eager_type_check(sme, 0, SM_ERR_TYPE, current_cx, sf);
      return (sm_object *)sm_new_double(obj0->line);
    }
    case SM_ERRSOURCE_EXPR: {
      sm_error *obj0 = (sm_error *)eager_type_check(sme, 0, SM_ERR_TYPE, current_cx, sf);
      return (sm_object *)obj0->source;
    }
    case SM_ERRMESSAGE_EXPR: {
      sm_error *obj0 = (sm_error *)eager_type_check(sme, 0, SM_ERR_TYPE, current_cx, sf);
      if (obj0->message)
        return (sm_object *)obj0->message;
      return (sm_object *)sms_false;
    }
    case SM_ERRNOTES_EXPR: {
      sm_error *obj0 = (sm_error *)eager_type_check(sme, 0, SM_ERR_TYPE, current_cx, sf);
      sm_cx    *notes;
      if (obj0->notes) {
        notes       = sm_new_cx(NULL);
        obj0->notes = notes;
      }
      return (sm_object *)obj0->notes;
    }
    default: // unrecognized expr gets returned without evaluation
      return input;
    } // End of expr case
  }
  case SM_META_TYPE: {
    return ((sm_meta *)input)->address;
  }
  case SM_SELF_TYPE: {
    return (sm_object *)current_cx;
  }
  case SM_SYMBOL_TYPE: {
    sm_symbol *sym      = (sm_symbol *)input;
    sm_string *var_name = sym->code_id; // codemap nickname optimization
    sm_object *sr       = sm_cx_get_far(current_cx, sym);
    if (sr)
      return sr;
    sm_symbol *title   = sm_new_symbol("varNotFound", 11);
    sm_string *message = sm_new_fstring_at(
      sms_heap, "%s was not found in cx saved to :noted on this err", &sym->name->content);
    sm_expr *envelope = sm_new_expr(SM_ARRAY_EXPR, input, NULL);
    return (sm_object *)sm_new_error_from_expr(title, message, envelope, NULL);
  }
  case SM_LOCAL_TYPE: {
    sm_local *local = (sm_local *)input;
    if (local->index >= sf->size) {
      sm_symbol *title   = sm_new_symbol("invalidLocal", 12);
      sm_string *message = sm_new_fstring_at(
        sms_heap,
        "This local variable points to element %i when the stack frame only has %i elements.",
        local->index, sf->size);
      // TODO: Add line/source to local objects, pass this data to a note cx for envelope
      sm_expr *envelope = sm_new_expr(SM_ARRAY_EXPR, input, NULL);
      return (sm_object *)sm_new_error_from_expr(title, message, envelope, NULL);
    }
    return sm_expr_get_arg(sf, local->index);
  }
  case SM_FUN_TYPE: {
    sm_fun *f = (sm_fun *)input;
    f         = (sm_fun *)sm_copy((sm_object *)f);
    f->parent = current_cx;
    return (sm_object *)f;
  }
  case SM_CX_TYPE: {
    sm_cx *cx = (sm_cx *)input;
    cx        = (sm_cx *)sm_copy((sm_object *)cx);
    return (sm_object *)cx;
  }
  case SM_ERR_TYPE: {
    // Run the error handler if it exists
    sm_cx   *scratch = *sm_global_lex_stack(NULL)->top;
    sm_fun  *fun     = (sm_fun *)sm_cx_get_far(scratch, sm_new_symbol("_errHandler", 11));
    sm_expr *sf      = sm_new_expr(SM_PARAM_LIST_EXPR, sm_copy(input), NULL);
    if (fun)
      return execute_fun(fun, sm_new_cx(NULL), sf);
    else
      return input;
  }
  default:
    return input;
  }
}
