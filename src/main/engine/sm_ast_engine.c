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

// The engine should return an error if this returns false
// and the next function will complain about type mismatch,
// going up the callstack, until we hit a try or not
// the checker is the type number of the function which is checking
bool expect_type(sm_object *arg_n, uint32_t arg_num, uint16_t arg_type, uint16_t checker) {
  if (arg_n->my_type != arg_type) {
    sm_string  *str  = sm_object_to_string(arg_n);
    const char *cstr = &(str->content);
    printf("Type Mismatch: %s requires argument %i to have type %s but %s has type %s\n",
           sm_global_fn_name(checker), arg_num, sm_global_type_name(arg_type), cstr,
           sm_global_type_name(arg_n->my_type));
    return false;
  }
  return true;
}

// Global true symbol
#define IS_TRUE (x)((void *)x == (void *)sms_true)
#define IS_FALSE(x) ((void *)x == (void *)sms_false)

// Recursive engine
sm_object *sm_engine_eval(sm_object *input, sm_cx *current_cx, sm_expr *sf) {
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
      sm_expr *result     = sm_new_expr_n(SM_ARRAY_EXPR, 9, 9);
      for (int i = 0; i < 9; i++)
        sm_expr_set_arg(result, i, (sm_object *)sm_new_double(time_array[i]));
      return (sm_object *)result;
      break;
    }
    case SM_GC_EXPR: {
      // This fails because it changes all of the pointers before the function returns.
      // sm_garbage_collect();
      printf("gc command is not implemented yet!\n");
      return (sm_object *)sms_false;
      break;
    }
    case SM_SLEEP_EXPR: {
      sm_object *obj0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      int        tms;
      if (!expect_type(obj0, 0, SM_DOUBLE_TYPE, SM_SLEEP_EXPR))
        return (sm_object *)sm_new_double(0);

      tms = (int)((sm_double *)obj0)->value;

      if (tms < 0) {
        printf("ERROR: sleep function was provided a negative value.\n");
        return (sm_object *)sm_new_double(0);
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
                                        (sm_object *)sm_new_double(status));
    }
    case SM_EXEC_EXPR: {
      sm_object *obj0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_string *path;
      if (!expect_type(obj0, 0, SM_STRING_TYPE, SM_EXEC_EXPR))
        return (sm_object *)sms_false;
      path = (sm_string *)obj0;
      // The system command leaves 8 bits for extra information
      // We do not need it, so we shift away the 8 bits
      int result = system(&path->content) >> 8;
      return (sm_object *)sm_new_double(result);
      break;
    }
    case SM_OS_GETENV_EXPR: {
      sm_object *obj0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_string *key;
      if (!expect_type(obj0, 0, SM_STRING_TYPE, SM_OS_GETENV_EXPR))
        return (sm_object *)sms_false;
      key          = (sm_string *)obj0;
      char *result = getenv(&key->content);
      if (!result)
        return (sm_object *)sms_false;
      return (sm_object *)sm_new_string(strlen(result), result);
      break;
    }
    case SM_OS_SETENV_EXPR: {
      sm_object *obj0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_object *obj1 = sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      sm_string *key;
      sm_string *value;
      if (!expect_type(obj0, 0, SM_STRING_TYPE, SM_OS_SETENV_EXPR))
        return (sm_object *)sms_false;
      key = (sm_string *)obj0;
      if (!expect_type(obj1, 1, SM_STRING_TYPE, SM_OS_SETENV_EXPR))
        return (sm_object *)sms_false;
      value      = (sm_string *)obj1;
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
      sm_expr *names_arr = sm_new_expr_n(SM_ARRAY_EXPR, i, i);
      sm_expr *types_arr = sm_new_expr_n(SM_ARRAY_EXPR, i, i);
      for (uint32_t names_i = 0; names_i < i; names_i++) {
        sm_expr_set_arg(names_arr, names_i, (sm_object *)entry_names[names_i]);
        if (entry_types[names_i] != 0)
          sm_expr_set_arg(types_arr, names_i, (sm_object *)sms_true);
        else
          sm_expr_set_arg(types_arr, names_i, (sm_object *)sms_false);
      }
      sm_expr *result =
        sm_new_expr_2(SM_ARRAY_EXPR, (sm_object *)names_arr, (sm_object *)types_arr);
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
      sm_object *obj0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_string *path;
      if (!expect_type(obj0, 0, SM_STRING_TYPE, SM_CD_EXPR))
        return (sm_object *)sm_new_double(0);
      path = (sm_string *)obj0;

      char *path_cstr = &path->content;
      if (chdir(path_cstr) == 0)
        return (sm_object *)sms_true;
      else
        return (sm_object *)sms_false;

      break;
    }
    case SM_DATE_STR_EXPR: {
      time_t     rawtime;
      struct tm *timeinfo;
      time(&rawtime);
      timeinfo          = localtime(&rawtime);
      sm_string *result = sm_new_string_manual(24);
      sm_strncpy(&(result->content), asctime(timeinfo), 24);
      (&result->content)[24] = '\0';
      return (sm_object *)result;
      break;
    }
    case SM_TIME_EXPR: {
      struct timeval t;
      gettimeofday(&t, NULL);
      sm_expr *result = sm_new_expr_n(SM_ARRAY_EXPR, 2, 2);
      sm_expr_set_arg(result, 0, (sm_object *)sm_new_double(t.tv_sec));
      sm_expr_set_arg(result, 1, (sm_object *)sm_new_double(t.tv_usec));
      return (sm_object *)result;
      break;
    }
    case SM_STR_FIND_EXPR: {
      sm_object *obj0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_string *haystack;
      if (!expect_type(obj0, 0, SM_STRING_TYPE, SM_STR_FIND_EXPR))
        return (sm_object *)sm_new_double(0);
      haystack        = (sm_string *)obj0;
      sm_object *obj1 = sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      sm_string *needle;
      if (!expect_type(obj1, 1, SM_STRING_TYPE, SM_STR_FIND_EXPR))
        return (sm_object *)sm_new_double(0);
      needle = (sm_string *)obj1;
      return (sm_object *)sm_str_find(haystack, needle);
      break;
    }
    case SM_STR_SPLIT_EXPR: {
      sm_object *obj0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_string *haystack;
      if (expect_type(obj0, 0, SM_STRING_TYPE, SM_STR_SPLIT_EXPR))
        return (sm_object *)sm_new_double(0);
      haystack        = (sm_string *)obj0;
      sm_object *obj1 = sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      sm_string *needle;
      if (expect_type(obj1, 1, SM_STRING_TYPE, SM_STR_SPLIT_EXPR))
        return (sm_object *)sm_new_double(0);
      needle = (sm_string *)obj1;
      return (sm_object *)sm_str_split(haystack, needle);
      break;
    }
    case SM_STR_PART_EXPR: {
      sm_object *obj0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_string *str0;
      if (!expect_type(obj0, 0, SM_STRING_TYPE, SM_STR_PART_EXPR))
        return (sm_object *)sm_new_string(0, "");
      str0            = (sm_string *)obj0;
      sm_object *obj1 = sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      sm_double *start;
      if (!expect_type(obj1, 1, SM_DOUBLE_TYPE, SM_STR_PART_EXPR))
        return (sm_object *)sm_new_string(0, "");
      start           = (sm_double *)obj1;
      sm_object *obj2 = sm_engine_eval(sm_expr_get_arg(sme, 2), current_cx, sf);
      sm_double *desired_len;
      if (!expect_type(obj2, 2, SM_DOUBLE_TYPE, SM_STR_PART_EXPR))
        return (sm_object *)sm_new_string(0, "");
      desired_len = (sm_double *)obj2;
      if (start->value < 0 || start->value >= str0->size) {
        printf("Error: Calling %s with out of range start value: %i.\n",
               sm_global_fn_name(SM_STR_PART_EXPR), (int)start->value);
        return (sm_object *)sm_new_string(0, "");
      }
      if (desired_len->value > str0->size - start->value) {
        printf("Error: Calling %s with out of range length value: %i.\n",
               sm_global_fn_name(SM_STR_PART_EXPR), (int)desired_len->value);
        return (sm_object *)sm_new_string(0, "");
      }
      sm_string *new_str = sm_new_string_manual((int)desired_len->value);
      char      *content = &(new_str->content);
      sm_strncpy(content, &(str0->content) + (int)start->value, (int)desired_len->value);
      return (sm_object *)new_str;
      break;
    }
    case SM_STR_CAT_EXPR: {
      sm_object *obj0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_string *str0;
      if (!expect_type(obj0, 0, SM_STRING_TYPE, SM_STR_CAT_EXPR))
        return (sm_object *)sm_new_string(0, "");
      str0            = (sm_string *)obj0;
      sm_object *obj1 = sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      sm_string *str1;
      if (!expect_type(obj1, 1, SM_STRING_TYPE, SM_STR_CAT_EXPR))
        return (sm_object *)sm_new_string(0, "");
      str1               = (sm_string *)obj1;
      sm_string *new_str = sm_new_string_manual(str0->size + str1->size);
      char      *content = &(new_str->content);
      sm_strncpy(content, &(str0->content), str0->size);
      sm_strncpy(content + str0->size, &(str1->content), str1->size);
      return (sm_object *)new_str;
      break;
    }
    case SM_STR_SIZE_EXPR: {
      sm_object *obj0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_string *str0;
      if (!expect_type(obj0, 0, SM_STRING_TYPE, SM_STR_SIZE_EXPR))
        return (sm_object *)sm_new_string(0, "");
      str0 = (sm_string *)obj0;
      return (sm_object *)sm_new_double(str0->size);
      break;
    }
    case SM_EXIT_EXPR: {
      sm_object *obj0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_double *num0;
      if (!expect_type(obj0, 0, SM_DOUBLE_TYPE, SM_EXIT_EXPR))
        return (sm_object *)sm_new_string(0, "");
      num0          = (sm_double *)obj0;
      int exit_code = num0->value;
      sm_signal_exit(exit_code);
      break;
    }
    case SM_LET_EXPR: {
      sm_symbol *sym   = (sm_symbol *)sm_expr_get_arg(sme, 0);
      sm_object *value = (sm_object *)sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      if (!expect_type((sm_object *)sym, 0, SM_SYMBOL_TYPE, SM_LET_EXPR))
        return (sm_object *)sms_false;
      if (sm_cx_let(current_cx, sym, value))
        return (sm_object *)value;
    }
    case SM_CX_SETPARENT_EXPR: {
      sm_cx *cx         = (sm_cx *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_cx *new_parent = (sm_cx *)sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      cx                = (sm_cx *)sm_copy((sm_object *)cx);
      if (!expect_type((sm_object *)cx, 0, SM_CX_TYPE, SM_CX_SETPARENT_EXPR))
        return (sm_object *)sms_false;
      if (!expect_type((sm_object *)new_parent, 1, SM_CX_TYPE, SM_CX_SETPARENT_EXPR))
        return (sm_object *)sms_false;
      cx->parent = new_parent;
      return (sm_object *)cx;
    }
    case SM_CX_LET_EXPR: {
      sm_cx     *cx    = (sm_cx *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_symbol *sym   = (sm_symbol *)sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      sm_object *value = (sm_object *)sm_engine_eval(sm_expr_get_arg(sme, 2), current_cx, sf);
      if (!expect_type((sm_object *)cx, 0, SM_CX_TYPE, SM_CX_LET_EXPR))
        return (sm_object *)sms_false;
      if (!expect_type((sm_object *)sym, 1, SM_SYMBOL_TYPE, SM_CX_LET_EXPR))
        return (sm_object *)sms_false;
      if (sm_cx_let(cx, sym, value))
        return (sm_object *)sms_true;
    }
    case SM_CX_GET_EXPR: {
      sm_cx     *cx  = (sm_cx *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_symbol *sym = (sm_symbol *)sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      if (!expect_type((sm_object *)cx, 0, SM_CX_TYPE, SM_CX_GET_EXPR))
        return (sm_object *)sms_false;
      if (!expect_type((sm_object *)sym, 1, SM_SYMBOL_TYPE, SM_CX_GET_EXPR))
        return (sm_object *)sms_false;
      sm_object *result = sm_cx_get(cx, &sym->code_id->content, sym->code_id->size);
      if (result)
        return result;
      else
        return (sm_object *)sms_false;
    }
    case SM_CX_HAS_EXPR: {
      sm_cx     *cx  = (sm_cx *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_symbol *sym = (sm_symbol *)sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      if (!expect_type((sm_object *)cx, 0, SM_CX_TYPE, SM_CX_HAS_EXPR))
        return (sm_object *)sms_false;
      if (!expect_type((sm_object *)sym, 1, SM_SYMBOL_TYPE, SM_CX_HAS_EXPR))
        return (sm_object *)sms_false;
      sm_object *result = sm_cx_get(cx, &sym->code_id->content, sym->code_id->size);
      if (result)
        return (sm_object *)sms_true;
      return (sm_object *)sms_false;
    }
    case SM_CX_GET_FAR_EXPR: {
      sm_cx     *cx  = (sm_cx *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_symbol *sym = (sm_symbol *)sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      if (!expect_type((sm_object *)cx, 0, SM_CX_TYPE, SM_CX_GET_FAR_EXPR))
        return (sm_object *)sms_false;
      if (!expect_type((sm_object *)sym, 1, SM_SYMBOL_TYPE, SM_CX_GET_FAR_EXPR))
        return (sm_object *)sms_false;
      sm_object *result = sm_cx_get_far(cx, &sym->code_id->content, sym->code_id->size);
      if (!result)
        return (sm_object *)sms_false;
      return result;
    }
    case SM_CX_HAS_FAR_EXPR: {
      sm_cx     *cx  = (sm_cx *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_symbol *sym = (sm_symbol *)sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      if (!expect_type((sm_object *)cx, 0, SM_CX_TYPE, SM_CX_GET_FAR_EXPR))
        return (sm_object *)sms_false;
      if (!expect_type((sm_object *)sym, 1, SM_SYMBOL_TYPE, SM_CX_GET_FAR_EXPR))
        return (sm_object *)sms_false;
      sm_object *result = sm_cx_get_far(cx, &sym->code_id->content, sym->code_id->size);
      if (!result)
        return (sm_object *)sms_false;
      return (sm_object *)sms_true;
    }
    case SM_CX_SET_EXPR: {
      sm_cx     *cx    = (sm_cx *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_symbol *sym   = (sm_symbol *)sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      sm_object *value = (sm_object *)sm_engine_eval(sm_expr_get_arg(sme, 2), current_cx, sf);
      if (!expect_type((sm_object *)cx, 0, SM_CX_TYPE, SM_CX_SET_EXPR))
        return (sm_object *)sms_false;
      if (!expect_type((sm_object *)sym, 1, SM_SYMBOL_TYPE, SM_CX_SET_EXPR))
        return (sm_object *)sms_false;
      if (sm_cx_set(cx, &sym->code_id->content, sym->code_id->size, value))
        return (sm_object *)sms_true;
      return (sm_object *)sms_false;
    }
    case SM_CX_DOT_EXPR: {
      sm_cx     *cx  = (sm_cx *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_symbol *sym = (sm_symbol *)sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      if (!expect_type((sm_object *)cx, 0, SM_CX_TYPE, SM_CX_DOT_EXPR))
        return (sm_object *)sms_false;
      if (!expect_type((sm_object *)sym, 1, SM_SYMBOL_TYPE, SM_CX_DOT_EXPR))
        return (sm_object *)sms_false;
      sm_object *retrieved = sm_cx_get_far(cx, &sym->code_id->content, sym->code_id->size);
      if (retrieved)
        return retrieved;
      return (sm_object *)sms_false;
    }
    case SM_CX_CLEAR_EXPR: {
      sm_cx *cx = (sm_cx *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      if (!expect_type((sm_object *)cx, 0, SM_CX_TYPE, SM_CX_CLEAR_EXPR))
        return (sm_object *)sms_false;
      sm_cx_clear(cx);
      return (sm_object *)sms_true;
    }
    case SM_CX_IMPORT_EXPR: {
      sm_cx *cxFrom = (sm_cx *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      if (!expect_type((sm_object *)cxFrom, 0, SM_CX_TYPE, SM_CX_IMPORT_EXPR))
        return (sm_object *)sms_false;
      sm_cx *cxTo = (sm_cx *)sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      if (!expect_type((sm_object *)cxTo, 1, SM_CX_TYPE, SM_CX_IMPORT_EXPR))
        return (sm_object *)sms_false;
      sm_cx_import(cxFrom, cxTo);
      return (sm_object *)sms_true;
    }
    case SM_CX_CONTAINING_EXPR: {
      sm_cx     *cx  = (sm_cx *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_symbol *sym = (sm_symbol *)sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      if (!expect_type((sm_object *)cx, 0, SM_CX_TYPE, SM_CX_CONTAINING_EXPR))
        return (sm_object *)sms_false;
      if (!expect_type((sm_object *)sym, 1, SM_SYMBOL_TYPE, SM_CX_CONTAINING_EXPR))
        return (sm_object *)sms_false;
      sm_cx *retrieved = sm_cx_get_container(cx, &sym->code_id->content, sym->code_id->size);
      if (retrieved)
        return (sm_object *)retrieved;
      return (sm_object *)sms_false;
    }
    case SM_CX_SIZE_EXPR: {
      sm_cx *cx = (sm_cx *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      if (!expect_type((sm_object *)cx, 0, SM_CX_TYPE, SM_CX_SIZE_EXPR))
        return (sm_object *)sms_false;
      int size = sm_cx_size(cx);
      return (sm_object *)sm_new_double(size);
    }
    case SM_RM_EXPR: {
      sm_symbol *sym = (sm_symbol *)sm_expr_get_arg(sme, 0);
      if (!expect_type((sm_object *)sym, 0, SM_SYMBOL_TYPE, SM_RM_EXPR))
        return (sm_object *)sms_false;
      bool success = sm_cx_rm(current_cx, &sym->code_id->content, sym->code_id->size);
      if (success == true)
        return (sm_object *)sms_true;
      return (sm_object *)sms_false;
    }
    case SM_CX_RM_EXPR: {
      sm_cx     *cx  = (sm_cx *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_symbol *sym = (sm_symbol *)sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      if (!expect_type((sm_object *)cx, 0, SM_CX_TYPE, SM_CX_RM_EXPR))
        return (sm_object *)sms_false;
      if (!expect_type((sm_object *)sym, 1, SM_SYMBOL_TYPE, SM_CX_RM_EXPR))
        return (sm_object *)sms_false;
      bool success = sm_cx_rm(cx, &sym->code_id->content, sym->code_id->size);
      if (success == true)
        return (sm_object *)sms_true;
      return (sm_object *)sms_false;
    }
    case SM_CX_KEYS_EXPR: {
      sm_cx *cx = (sm_cx *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      if (!expect_type((sm_object *)cx, 0, SM_CX_TYPE, SM_CX_KEYS_EXPR))
        return (sm_object *)sms_false;
      sm_expr *success =
        sm_node_keys(cx->content, sm_new_stack_obj(32), sm_new_expr_n(SM_ARRAY_EXPR, 0, 0));
      if (success)
        return (sm_object *)success;
      return (sm_object *)sms_false;
    }
    case SM_CX_VALUES_EXPR: {
      sm_cx *cx = (sm_cx *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      if (!expect_type((sm_object *)cx, 0, SM_CX_TYPE, SM_CX_VALUES_EXPR))
        return (sm_object *)sms_false;
      sm_expr *success = sm_node_values(cx->content, sm_new_expr_n(SM_ARRAY_EXPR, 0, 0));
      if (success)
        return (sm_object *)success;
      return (sm_object *)sms_false;
    }
    case SM_FN_XP_EXPR: {
      sm_fun *fun = (sm_fun *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      if (!expect_type((sm_object *)fun, 0, SM_FUN_TYPE, SM_FN_XP_EXPR))
        return (sm_object *)sms_false;
      return sm_unlocalize((sm_object *)fun->content);
    }
    case SM_FN_SETXP_EXPR: {
      sm_fun *fun = (sm_fun *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      if (!expect_type((sm_object *)fun, 0, SM_FUN_TYPE, SM_FN_SETXP_EXPR))
        return (sm_object *)sms_false;
      fun->content = sm_localize(sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf), fun);
      return (sm_object *)fun;
    }
    case SM_FN_PARAMS_EXPR: {
      sm_fun *fun = (sm_fun *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      if (!expect_type((sm_object *)fun, 0, SM_FUN_TYPE, SM_FN_PARAMS_EXPR))
        return (sm_object *)sms_false;
      sm_expr *result = sm_new_expr_n(SM_PARAM_LIST_EXPR, fun->num_params, fun->num_params);
      for (uint32_t i = 0; i < fun->num_params; i++) {
        sm_string *fn_name = sm_fun_get_param(fun, i)->name;
        sm_expr_set_arg(result, i, (sm_object *)sm_new_symbol(&(fn_name->content), fn_name->size));
      }
      return (sm_object *)result;
    } break;
    case SM_FN_SETPARAMS_EXPR: {
      sm_fun  *fun    = (sm_fun *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_expr *params = (sm_expr *)sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      if (!expect_type((sm_object *)fun, 0, SM_FUN_TYPE, SM_FN_SETPARAMS_EXPR))
        return (sm_object *)sms_false;
      if (!expect_type((sm_object *)params, 1, SM_EXPR_TYPE, SM_FN_SETPARAMS_EXPR))
        return (sm_object *)sms_false;
      sm_fun *new_fun = sm_new_fun(fun->parent, fun->num_params, fun->content);
      // Setting the parameters of a new function
      for (uint32_t i = 0; i < params->size; i++) {
        sm_symbol *sym = (sm_symbol *)sm_expr_get_arg(params, i);
        if (!expect_type((sm_object *)sym, i, SM_SYMBOL_TYPE, SM_FN_SETPARAMS_EXPR))
          break;
        sm_fun_set_param(new_fun, i, sym->name, NULL, 0);
      }
      // Relocalizing the AST
      new_fun->content    = sm_unlocalize(new_fun->content);
      new_fun->num_params = params->size;
      new_fun->content    = sm_localize(new_fun->content, new_fun);
      return (sm_object *)new_fun;
    } break;
    case SM_FN_PARENT_EXPR: {
      sm_fun *fun = (sm_fun *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      if (!expect_type((sm_object *)fun, 0, SM_FUN_TYPE, SM_FN_PARENT_EXPR))
        return (sm_object *)sms_false;
      return (sm_object *)fun->parent;
    }
    case SM_FN_SETPARENT_EXPR: {
      sm_fun *fun        = (sm_fun *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_cx  *new_parent = (sm_cx *)sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      fun                = (sm_fun *)sm_copy((sm_object *)fun);
      if (!expect_type((sm_object *)fun, 0, SM_FUN_TYPE, SM_FN_SETPARENT_EXPR))
        return (sm_object *)sms_false;
      if (!expect_type((sm_object *)new_parent, 1, SM_CX_TYPE, SM_FN_SETPARENT_EXPR))
        return (sm_object *)sms_false;
      fun->parent = new_parent;
      return (sm_object *)fun;
    }
    case SM_XP_OP_EXPR: {
      sm_object *obj0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      if (!expect_type(obj0, 0, SM_EXPR_TYPE, SM_XP_OP_EXPR))
        return (sm_object *)sms_false;
      sm_expr *expression = (sm_expr *)obj0;
      return (sm_object *)sm_new_double(expression->op);
    }
    case SM_XP_SET_OP_EXPR: {
      sm_object *obj0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      if (!expect_type(obj0, 0, SM_EXPR_TYPE, SM_XP_SET_OP_EXPR))
        return (sm_object *)sms_false;
      sm_object *obj1 = sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      if (!expect_type(obj1, 1, SM_DOUBLE_TYPE, SM_XP_SET_OP_EXPR))
        return (sm_object *)sms_false;
      sm_expr *expression = (sm_expr *)obj0;
      expression          = (sm_expr *)sm_copy((sm_object *)expression);
      sm_double *given_op = (sm_double *)obj1;
      expression->op      = (int)((sm_double *)given_op)->value;
      return (sm_object *)expression;
    }
    case SM_XP_OP_STR_EXPR: {
      sm_object *obj0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      if (!expect_type(obj0, 0, SM_DOUBLE_TYPE, SM_XP_OP_STR_EXPR))
        return (sm_object *)sms_false;
      sm_double *op_num = (sm_double *)obj0;
      return (sm_object *)sm_new_string(sm_global_fn_name_len((int)op_num->value),
                                        sm_global_fn_name((int)op_num->value));
    }
    case SM_STR_ESCAPE_EXPR: {
      sm_object *obj0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_string *str0;
      if (!expect_type(obj0, 0, SM_STRING_TYPE, SM_STR_ESCAPE_EXPR))
        return (sm_object *)sms_false;
      str0 = (sm_string *)obj0;
      return (sm_object *)sm_string_escape(str0);
    }
    case SM_INPUT_EXPR: {
      char input_str[500];
      fgets(input_str, sizeof(input_str), stdin);
      // we remove the trailing newline character
      return (sm_object *)sm_new_string(strlen(input_str) - 1, input_str);
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
      if (expect_type(obj0, 0, SM_DOUBLE_TYPE, SM_ROUND_EXPR))
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
      if (expect_type(obj0, 0, SM_DOUBLE_TYPE, SM_FLOOR_EXPR))
        number = (sm_double *)obj0;
      else
        return (sm_object *)sm_new_string(0, "");
      double val = number->value;
      return (sm_object *)sm_new_double(floor(val));
    }
    case SM_CEIL_EXPR: {
      sm_object *obj0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_double *number;
      if (expect_type(obj0, 0, SM_DOUBLE_TYPE, SM_CEIL_EXPR))
        number = (sm_double *)obj0;
      else
        return (sm_object *)sm_new_string(0, "");
      double val = number->value;
      return (sm_object *)sm_new_double(ceil(val));
    }
    case SM_MOD_EXPR: {
      sm_object *obj0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_object *obj1 = sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      if (!expect_type(obj0, 0, SM_DOUBLE_TYPE, SM_MOD_EXPR))
        return (sm_object *)sms_false;
      sm_double *num0 = (sm_double *)obj0;
      if (!expect_type(obj1, 1, SM_DOUBLE_TYPE, SM_MOD_EXPR))
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
      if (!expect_type(obj0, 0, SM_DOUBLE_TYPE, SM_SEED_EXPR))
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
      if (!expect_type(evaluated_fname, 0, SM_STRING_TYPE, SM_FILE_WRITE_EXPR))
        return (sm_object *)sms_false;
      fname_str        = (sm_string *)evaluated_fname;
      char *fname_cstr = &(fname_str->content);
      // obtain the content to write
      sm_object *evaluated_content = sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      sm_string *content_str;
      if (!expect_type(evaluated_content, 1, SM_STRING_TYPE, SM_FILE_WRITE_EXPR))
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
      if (!expect_type(evaluated_fname, 0, SM_STRING_TYPE, SM_FILE_APPEND_EXPR))
        return (sm_object *)sms_false;
      fname_str        = (sm_string *)evaluated_fname;
      char *fname_cstr = &(fname_str->content);
      // obtain the content to write
      sm_object *evaluated_content = sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      sm_string *content_str;
      if (!expect_type(evaluated_content, 1, SM_STRING_TYPE, SM_FILE_WRITE_EXPR))
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
      if (!expect_type(evaluated, 0, SM_STRING_TYPE, SM_FILE_READ_EXPR))
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
      if (!expect_type(evaluated, 0, SM_STRING_TYPE, SM_FILE_PART_EXPR))
        return (sm_object *)sms_false;
      fname_str        = (sm_string *)evaluated;
      char *fname_cstr = &(fname_str->content);
      evaluated        = sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      sm_double *start_pos;
      if (!expect_type(evaluated, 1, SM_DOUBLE_TYPE, SM_FILE_PART_EXPR))
        return (sm_object *)sms_false;
      start_pos = (sm_double *)evaluated;
      evaluated = sm_engine_eval(sm_expr_get_arg(sme, 2), current_cx, sf);
      sm_double *length;
      if (!expect_type(evaluated, 2, SM_DOUBLE_TYPE, SM_FILE_PART_EXPR))
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
      if (!expect_type(evaluated, 0, SM_STRING_TYPE, SM_FILE_EXISTS_EXPR))
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
      if (!expect_type(evaluated, 0, SM_STRING_TYPE, SM_FILE_RM_EXPR))
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
      if (!expect_type(evaluated, 0, SM_STRING_TYPE, SM_FILE_STAT_EXPR))
        return (sm_object *)sms_false;
      fname_str              = (sm_string *)evaluated;
      char       *fname_cstr = &(fname_str->content);
      struct stat filestat;
      sm_expr    *output;
      if (stat(fname_cstr, &filestat) == 0) {
        // build an array with stat information.
        output = sm_new_expr_n(SM_ARRAY_EXPR, 16, 16);
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
      if (!expect_type(evaluated, 0, SM_STRING_TYPE, SM_FILE_PARSE_EXPR))
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
      if (!expect_type(evaluated, 0, SM_STRING_TYPE, SM_PARSE_EXPR))
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
    case SM_TO_STRING_EXPR: {
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
      if (!expect_type(evaluated, 0, SM_CX_TYPE, SM_CX_EVAL_EXPR))
        return (sm_object *)sms_true;
      where_to_eval = (sm_cx *)evaluated;
      return sm_engine_eval(obj1, where_to_eval, sf);
    }
    case SM_PUT_EXPR: {
      sm_object *evaluated = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_string *str;
      if (!expect_type(evaluated, 0, SM_STRING_TYPE, SM_PUT_EXPR))
        return (sm_object *)sms_false;
      str = (sm_string *)evaluated;
      for (uint32_t i = 0; i < str->size; i++)
        putchar((&str->content)[i]);
      putchar('\0');
      fflush(stdout);
      return (sm_object *)sms_true;
      break;
    }
    case SM_PUTLN_EXPR: {
      sm_object *evaluated = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_string *str;
      if (!expect_type(evaluated, 0, SM_STRING_TYPE, SM_PUTLN_EXPR))
        return (sm_object *)sms_false;
      str = (sm_string *)evaluated;
      for (uint32_t i = 0; i < str->size; i++)
        putchar((&str->content)[i]);
      putchar('\n');
      putchar('\0');
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
      return (sm_object *)sms_true;
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
      return (sm_object *)sms_true;
      break;
    }
    case SM_RETURN_EXPR: {
      sm_object *to_return = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      return (sm_object *)sm_new_return(to_return);
    }
    case SM_SIZE_EXPR: {
      sm_object *base_obj = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_expr   *arr;
      if (!expect_type(base_obj, 0, SM_EXPR_TYPE, SM_SIZE_EXPR))
        return (sm_object *)sms_false;
      arr = (sm_expr *)base_obj;
      return (sm_object *)sm_new_double(arr->size);
    }
    case SM_MAP_EXPR: {
      // expecting a unary func
      sm_object *obj0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_fun    *fun;
      if (!expect_type(obj0, 0, SM_FUN_TYPE, SM_MAP_EXPR))
        return (sm_object *)sms_false;
      fun             = (sm_fun *)obj0;
      sm_object *obj1 = sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      sm_expr   *arr;
      if (!expect_type(obj1, 1, SM_EXPR_TYPE, SM_MAP_EXPR))
        return (sm_object *)sms_false;
      arr             = (sm_expr *)obj1;
      sm_expr *output = sm_new_expr_n(arr->op, arr->size, arr->size);
      for (uint32_t i = 0; i < arr->size; i++) {
        sm_object *current_obj = sm_expr_get_arg(arr, i);
        sm_expr   *new_sf      = sm_new_expr(SM_PARAM_LIST_EXPR, current_obj);

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
      if (!expect_type(obj0, 0, SM_FUN_TYPE, SM_REDUCE_EXPR))
        return (sm_object *)sms_false;
      fun = (sm_fun *)obj0;
      // evaluating the expression to reduce
      sm_object *obj1 = sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      sm_expr   *arr;
      if (!expect_type(obj1, 1, SM_EXPR_TYPE, SM_REDUCE_EXPR))
        return (sm_object *)sms_false;
      arr = (sm_expr *)obj1;
      // initial value for reduction
      sm_object *initial = sm_engine_eval(sm_expr_get_arg(sme, 2), current_cx, sf);
      // reducing the expression
      sm_object *result = initial;
      // Evaluating with a reusable stack frame: ( result , current_ob )
      sm_expr *reusable = sm_new_expr_2(SM_PARAM_LIST_EXPR, result, NULL);
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
      if (!expect_type(base_obj, 0, SM_EXPR_TYPE, SM_INDEX_EXPR))
        return (sm_object *)sms_false;
      arr                  = (sm_expr *)base_obj;
      sm_object *index_obj = sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      sm_double *index_double;
      if (!expect_type(index_obj, 1, SM_DOUBLE_TYPE, SM_INDEX_EXPR))
        return (sm_object *)sm_new_string(0, "");
      index_double   = (sm_double *)index_obj;
      uint32_t index = (int)index_double->value;
      if (arr->size < index + 1 || index_double->value < 0) {
        printf("Error: Index out of range: %i . Array size is %i\n", index, arr->size);
        return (sm_object *)sms_false;
      }
      return sm_expr_get_arg(arr, index);
    }
    case SM_DOT_EXPR: {
      sm_object *base_obj   = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_symbol *field_sym  = (sm_symbol *)sm_expr_get_arg(sme, 1);
      sm_string *field_name = field_sym->code_id; // codemap opt
      sm_cx     *base_cx;
      if (!expect_type(base_obj, 0, SM_CX_TYPE, SM_DOT_EXPR))
        return (sm_object *)sms_false;
      base_cx       = (sm_cx *)base_obj;
      sm_object *sr = sm_cx_get_far(base_cx, &field_name->content, field_name->size);
      if (sr == NULL) {
        sm_string *base_str = sm_object_to_string(base_obj);
        printf("Error: Could not find variable: %s within %s\n", &(field_name->content),
               &(base_str->content));
        return (sm_object *)sms_false;
      }
      // return sm_cx_entries(sr.context)[sr.index].value;
      return (sm_object *)sr;
    }
    case SM_PARENT_EXPR: {
      sm_object *base_obj = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_cx     *base_cx;
      if (!expect_type(base_obj, 0, SM_CX_TYPE, SM_PARENT_EXPR))
        return (sm_object *)sms_false;
      base_cx = (sm_cx *)base_obj;
      if (base_cx->parent == NULL) {
        return (sm_object *)sms_false;
      }
      return (sm_object *)base_cx->parent;
    }
    case SM_DIFF_EXPR: {
      sm_object *evaluated0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_object *evaluated1 = sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      sm_symbol *sym1;
      if (!expect_type(evaluated1, 1, SM_SYMBOL_TYPE, SM_DIFF_EXPR))
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
      if (!expect_type(obj0, 0, SM_SYMBOL_TYPE, SM_ASSIGN_EXPR))
        return (sm_object *)sms_false;
      sym = (sm_symbol *)obj0;
      if (!sm_cx_set(current_cx, &sym->code_id->content, sym->code_id->size, value))
        return (sm_object *)sms_false;
      return value;
    }
    case SM_ASSIGN_DOT_EXPR: {
      sm_object *obj0  = sm_expr_get_arg(sme, 0);
      sm_object *value = (sm_object *)sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      if (!expect_type(obj0, 0, SM_EXPR_TYPE, SM_ASSIGN_DOT_EXPR))
        return (sm_object *)sms_false;
      sm_expr *dot_expr = (sm_expr *)obj0;
      if (dot_expr->op != SM_DOT_EXPR) {
        char *error_msg = "Error: Incorrectly formatted AST.";
        int   len       = strlen(error_msg);
        printf("%s\n", error_msg);
        return (sm_object *)sm_new_error(sm_new_string(len, error_msg), sm_new_string(0, ""), 0);
      }
      sm_cx     *predot  = (sm_cx *)sm_engine_eval(sm_expr_get_arg(dot_expr, 0), current_cx, sf);
      sm_symbol *postdot = (sm_symbol *)sm_expr_get_arg(dot_expr, 1);
      if (!expect_type((sm_object *)predot, 0, SM_CX_TYPE, SM_ASSIGN_DOT_EXPR))
        return (sm_object *)sms_false;
      if (!sm_cx_let(predot, postdot, value))
        return (sm_object *)sms_false;
      return (sm_object *)sms_true;
    }
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
      sm_object *obj2 = sm_engine_eval(sm_expr_get_arg(sme, 2), current_cx, sf);
      sm_object *obj0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_object *obj1 = sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      if (!expect_type(obj0, 0, SM_EXPR_TYPE, SM_ASSIGN_INDEX_EXPR))
        return (sm_object *)sms_false;
      sm_expr *arr = (sm_expr *)obj0;
      if (!expect_type(obj1, 1, SM_DOUBLE_TYPE, SM_ASSIGN_INDEX_EXPR))
        return (sm_object *)sms_false;
      sm_double *index = (sm_double *)obj1;
      if (index->value >= arr->size || index->value < -0) {
        printf("Error: Index out of range: %i\n", (int)index->value);
        return (sm_object *)sm_new_error(sm_new_string(19, "Index out of range"),
                                         sm_new_string(1, "?"), 0);
      }
      sm_expr_set_arg(arr, index->value, obj2);
      return (sm_object *)sms_true;
      break;
    }
    case SM_PLUS_EXPR: {
      sm_double *obj0 = (sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_double *obj1 = (sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      if (!expect_type((sm_object *)obj0, 0, SM_DOUBLE_TYPE, SM_PLUS_EXPR))
        return (sm_object *)sms_false;
      if (!expect_type((sm_object *)obj1, 1, SM_DOUBLE_TYPE, SM_PLUS_EXPR))
        return (sm_object *)sms_false;
      return (sm_object *)sm_new_double(obj0->value + obj1->value);
    }
    case SM_MINUS_EXPR: {
      sm_double *obj0 = (sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_double *obj1 = (sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      if (!expect_type((sm_object *)obj0, 0, SM_DOUBLE_TYPE, SM_MINUS_EXPR))
        return (sm_object *)sms_false;
      if (!expect_type((sm_object *)obj1, 1, SM_DOUBLE_TYPE, SM_MINUS_EXPR))
        return (sm_object *)sms_false;
      return (sm_object *)sm_new_double(obj0->value - obj1->value);
    }
    case SM_TIMES_EXPR: {
      sm_double *obj0 = (sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_double *obj1 = (sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      if (!expect_type((sm_object *)obj0, 0, SM_DOUBLE_TYPE, SM_TIMES_EXPR))
        return (sm_object *)sms_false;
      if (!expect_type((sm_object *)obj1, 1, SM_DOUBLE_TYPE, SM_TIMES_EXPR))
        return (sm_object *)sms_false;
      return (sm_object *)sm_new_double(obj0->value * obj1->value);
    }
    case SM_DIVIDE_EXPR: {
      sm_double *obj0 = (sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_double *obj1 = (sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      if (!expect_type((sm_object *)obj0, 0, SM_DOUBLE_TYPE, SM_DIVIDE_EXPR))
        return (sm_object *)sms_false;
      if (!expect_type((sm_object *)obj1, 1, SM_DOUBLE_TYPE, SM_DIVIDE_EXPR))
        return (sm_object *)sms_false;
      return (sm_object *)sm_new_double(obj0->value / obj1->value);
    }
    case SM_POW_EXPR: {
      sm_object *arg0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_object *arg1 = sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      sm_double *num0;
      if (!expect_type(arg0, 0, SM_DOUBLE_TYPE, SM_POW_EXPR))
        return (sm_object *)sms_false;
      num0 = (sm_double *)arg0;
      sm_double *num1;
      if (!expect_type(arg1, 1, SM_DOUBLE_TYPE, SM_POW_EXPR))
        return (sm_object *)sms_false;
      num1 = (sm_double *)arg1;
      return (sm_object *)sm_new_double(pow(num0->value, num1->value));
    }
    case SM_SIN_EXPR: {
      sm_object *arg0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_double *num0;
      if (!expect_type(arg0, 0, SM_DOUBLE_TYPE, SM_SIN_EXPR))
        return (sm_object *)sms_false;
      num0 = (sm_double *)arg0;
      return (sm_object *)sm_new_double(sin(num0->value));
    }
    case SM_COS_EXPR: {
      sm_object *arg0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_double *num0;
      if (!expect_type(arg0, 0, SM_DOUBLE_TYPE, SM_COS_EXPR))
        return (sm_object *)sms_false;
      num0 = (sm_double *)arg0;
      return (sm_object *)sm_new_double(cos(num0->value));
    }
    case SM_TAN_EXPR: {
      sm_object *arg0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_double *num0;
      if (!expect_type(arg0, 0, SM_DOUBLE_TYPE, SM_TAN_EXPR))
        return (sm_object *)sms_false;
      num0 = (sm_double *)arg0;
      return (sm_object *)sm_new_double(tan(num0->value));
    }
    case SM_ASIN_EXPR: {
      sm_object *arg0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_double *num0;
      if (!expect_type(arg0, 0, SM_DOUBLE_TYPE, SM_SIN_EXPR))
        return (sm_object *)sms_false;
      num0     = (sm_double *)arg0;
      double x = num0->value;
      return (sm_object *)sm_new_double(asin(x));
    }
    case SM_ACOS_EXPR: {
      sm_object *arg0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_double *num0;
      if (!expect_type(arg0, 0, SM_DOUBLE_TYPE, SM_COS_EXPR))
        return (sm_object *)sms_false;
      num0     = (sm_double *)arg0;
      double x = num0->value;
      return (sm_object *)sm_new_double(acos(x));
      printf("acos is not implemented yet.\n");
      return (sm_object *)sm_new_double((num0->value));
    }
    case SM_ATAN_EXPR: {
      sm_object *arg0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_double *num0;
      if (!expect_type(arg0, 0, SM_DOUBLE_TYPE, SM_TAN_EXPR))
        return (sm_object *)sms_false;
      num0     = (sm_double *)arg0;
      double x = num0->value;
      return (sm_object *)sm_new_double(atan(x));
      printf("atan is not implemented yet.\n");
      return (sm_object *)sm_new_double((num0->value));
    }
    case SM_SEC_EXPR: {
      sm_object *arg0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_double *num0;
      if (!expect_type(arg0, 0, SM_DOUBLE_TYPE, SM_SEC_EXPR))
        return (sm_object *)sms_false;
      num0 = (sm_double *)arg0;
      return (sm_object *)sm_new_double(1.0 / cos(num0->value));
    }
    case SM_CSC_EXPR: {
      sm_object *arg0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_double *num0;
      if (!expect_type(arg0, 0, SM_DOUBLE_TYPE, SM_CSC_EXPR))
        return (sm_object *)sms_false;
      num0 = (sm_double *)arg0;
      return (sm_object *)sm_new_double(1.0 / sin(num0->value));
    }
    case SM_COT_EXPR: {
      sm_object *arg0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_double *num0;
      if (!expect_type(arg0, 0, SM_DOUBLE_TYPE, SM_COT_EXPR))
        return (sm_object *)sms_false;
      num0 = (sm_double *)arg0;
      return (sm_object *)sm_new_double(1.0 / tan(num0->value));
    }
    case SM_SINH_EXPR: {
      sm_object *arg0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_double *num0;
      if (!expect_type(arg0, 0, SM_DOUBLE_TYPE, SM_SINH_EXPR))
        return (sm_object *)sms_false;
      num0 = (sm_double *)arg0;
      return (sm_object *)sm_new_double(sinh(num0->value));
    }
    case SM_COSH_EXPR: {
      sm_object *arg0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_double *num0;
      if (!expect_type(arg0, 0, SM_DOUBLE_TYPE, SM_COSH_EXPR))
        return (sm_object *)sms_false;
      num0 = (sm_double *)arg0;
      return (sm_object *)sm_new_double(cosh(num0->value));
    }
    case SM_TANH_EXPR: {
      sm_object *arg0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_double *num0;
      if (!expect_type(arg0, 0, SM_DOUBLE_TYPE, SM_TANH_EXPR))
        return (sm_object *)sms_false;
      num0 = (sm_double *)arg0;
      return (sm_object *)sm_new_double(tanh(num0->value));
    }
    case SM_SECH_EXPR: {
      sm_object *arg0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_double *num0;
      if (!expect_type(arg0, 0, SM_DOUBLE_TYPE, SM_TANH_EXPR))
        return (sm_object *)sms_false;
      num0 = (sm_double *)arg0;
      return (sm_object *)sm_new_double(1.0 / cosh(num0->value));
    }
    case SM_CSCH_EXPR: {
      sm_object *arg0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_double *num0;
      if (!expect_type(arg0, 0, SM_DOUBLE_TYPE, SM_TANH_EXPR))
        return (sm_object *)sms_false;
      num0 = (sm_double *)arg0;
      return (sm_object *)sm_new_double(1.0 / sinh(num0->value));
    }
    case SM_COTH_EXPR: {
      sm_object *arg0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_double *num0;
      if (!expect_type(arg0, 0, SM_DOUBLE_TYPE, SM_TANH_EXPR))
        return (sm_object *)sms_false;
      num0 = (sm_double *)arg0;
      return (sm_object *)sm_new_double(1.0 / tanh(num0->value));
    }
    case SM_LN_EXPR: {
      sm_object *arg0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_double *num0;
      if (!expect_type(arg0, 0, SM_DOUBLE_TYPE, SM_TANH_EXPR))
        return (sm_object *)sms_false;
      num0 = (sm_double *)arg0;
      return (sm_object *)sm_new_double(log(num0->value));
    }
    case SM_LOG_EXPR: {
      sm_object *arg0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_object *arg1 = sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      sm_double *num0;
      if (!expect_type(arg0, 0, SM_DOUBLE_TYPE, SM_LOG_EXPR))
        return (sm_object *)sms_false;
      num0 = (sm_double *)arg0;
      sm_double *num1;
      if (!expect_type(arg1, 1, SM_DOUBLE_TYPE, SM_LOG_EXPR))
        return (sm_object *)sms_false;
      num1 = (sm_double *)arg1;
      return (sm_object *)sm_new_double(log(num1->value) / log(num0->value));
    }
    case SM_EXP_EXPR: {
      sm_object *arg0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_double *num0;
      if (!expect_type(arg0, 0, SM_DOUBLE_TYPE, SM_TANH_EXPR))
        return (sm_object *)sms_false;
      num0 = (sm_double *)arg0;
      return (sm_object *)sm_new_double(exp(num0->value));
    }
    case SM_SQRT_EXPR: {
      sm_object *arg0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_double *num0;
      if (!expect_type(arg0, 0, SM_DOUBLE_TYPE, SM_TANH_EXPR))
        return (sm_object *)sms_false;
      num0 = (sm_double *)arg0;
      return (sm_object *)sm_new_double(sqrt(num0->value));
    }
    case SM_ABS_EXPR: {
      sm_object *arg0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_double *num0;
      if (!expect_type(arg0, 0, SM_DOUBLE_TYPE, SM_TANH_EXPR))
        return (sm_object *)sms_false;
      num0 = (sm_double *)arg0;
      return (sm_object *)sm_new_double(num0->value < 0 ? -1 * num0->value : num0->value);
    }
    case SM_IF_EXPR: {
      sm_object *condition_result = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      if (!IS_FALSE(condition_result))
        return sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      return (sm_object *)sms_false;
    }
    case SM_IF_ELSE_EXPR: {
      sm_object *condition_result = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      if (!IS_FALSE(condition_result))
        return sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      return sm_engine_eval(sm_expr_get_arg(sme, 2), current_cx, sf);
    }
    case SM_ARRAY_EXPR: {
      sm_expr *new_arr = sm_new_expr_n(SM_ARRAY_EXPR, sme->size, sme->size);
      for (uint32_t i = 0; i < sme->size; i++) {
        sm_object *new_val = sm_engine_eval(sm_expr_get_arg(sme, i), current_cx, sf);
        sm_expr_set_arg(new_arr, i, new_val);
      }
      return (sm_object *)new_arr;
    }
    case SM_PARAM_LIST_EXPR: {
      sm_expr *new_arr = sm_new_expr_n(SM_PARAM_LIST_EXPR, sme->size, sme->size);
      for (uint32_t i = 0; i < sme->size; i++) {
        sm_object *new_val = sm_engine_eval(sm_expr_get_arg(sme, i), current_cx, sf);
        sm_expr_set_arg(new_arr, i, new_val);
      }
      return (sm_object *)new_arr;
    }
    case SM_LT_EXPR: {
      sm_object *obj1 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_object *obj2 = sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      if (obj1->my_type == SM_DOUBLE_TYPE) {
        if (obj2->my_type == SM_DOUBLE_TYPE) {
          double v1 = ((sm_double *)obj1)->value;
          double v2 = ((sm_double *)obj2)->value;
          if (v1 < v2)
            return (sm_object *)sms_true;
          else
            return (sm_object *)sms_false;
        }
        return (sm_object *)sms_false;
      }
    }
    case SM_GT_EXPR: {
      sm_object *obj1 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_object *obj2 = sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      if (obj1->my_type == SM_DOUBLE_TYPE) {
        if (obj2->my_type == SM_DOUBLE_TYPE) {
          double v1 = ((sm_double *)obj1)->value;
          double v2 = ((sm_double *)obj2)->value;
          if (v1 > v2)
            return (sm_object *)sms_true;
          else
            return (sm_object *)sms_false;
        }
      }
      return (sm_object *)sms_false;
    }
    case SM_EQEQ_EXPR: {
      sm_object *obj1 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_object *obj2 = sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      if (obj1 == obj2) {
        return (sm_object *)sms_true;
      }

      if (sm_object_eq(obj1, obj2))
        return (sm_object *)sms_true;
      else
        return (sm_object *)sms_false;
    }
    case SM_IS_EXPR: {
      sm_object *obj1 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_object *obj2 = sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      if (obj1 == obj2)
        return (sm_object *)sms_true;
      else
        return (sm_object *)sms_false;
    }
    case SM_GT_EQ_EXPR: {
      sm_object *obj0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_object *obj1 = sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      if (obj0->my_type != SM_DOUBLE_TYPE) {
        sm_string *str  = sm_object_to_string(obj0);
        char      *cstr = &(str->content);
        printf(">= failed. First argument is not a number: %s\n", cstr);
      }
      if (obj1->my_type != SM_DOUBLE_TYPE) {
        sm_string *str  = sm_object_to_string(obj1);
        char      *cstr = &(str->content);
        printf(">= failed. Second argument is not a number: %s\n", cstr);
      }
      double val0 = ((sm_double *)obj0)->value;
      double val1 = ((sm_double *)obj1)->value;
      if (val0 >= val1)
        return (sm_object *)sms_true;
      else
        return (sm_object *)sms_false;
    }
    case SM_LT_EQ_EXPR: {
      sm_object *obj0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_object *obj1 = sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      if (obj0->my_type != SM_DOUBLE_TYPE) {
        sm_string *str  = sm_object_to_string(obj0);
        char      *cstr = &(str->content);
        printf("<= failed. First argument is not a number: %s\n", cstr);
      }
      if (obj1->my_type != SM_DOUBLE_TYPE) {
        sm_string *str  = sm_object_to_string(obj1);
        char      *cstr = &(str->content);
        printf("<= failed. Second argument is not a number: %s\n", cstr);
      }
      double val0 = ((sm_double *)obj0)->value;
      double val1 = ((sm_double *)obj1)->value;
      if (val0 <= val1)
        return (sm_object *)sms_true;
      else
        return (sm_object *)sms_false;
    }
    case SM_RUNTIME_META_EXPR: {
      sm_object *obj0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      return (sm_object *)sm_new_meta(obj0, current_cx);
    }
    default:
      return input;
    } // End of expr case
  }
  case SM_META_TYPE: {
    return ((sm_meta *)input)->address;
  }
  case SM_SELF_TYPE: {
    return (sm_object *)current_cx;
  }
  case SM_PRIMITIVE_TYPE: {
    printf("Primitives are not developed yet\n");
    fflush(stdout);
    return input;
  }
  case SM_SYMBOL_TYPE: {
    sm_symbol *sym      = (sm_symbol *)input;
    sm_string *var_name = sym->code_id; // codemap nickname optimization
    sm_object *sr       = sm_cx_get_far(current_cx, &(var_name->content), var_name->size);
    if (sr)
      return sr;
    else {
      sm_string *base_str = sm_object_to_string((sm_object *)current_cx);
      printf("Error: Could not find variable: %s within %s\n", &(sym->name->content),
             &(base_str->content));
      return (sm_object *)sm_new_double(0);
    }
  }
  case SM_LOCAL_TYPE: {
    sm_local *local = (sm_local *)input;
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
  case SM_ERROR_TYPE: {
    // TODO: Add callstack info here.
    return input;
  }
  default:
    return input;
  }
}
