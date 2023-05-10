// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"
#include "sys/time.h"
#include <dirent.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

// The engine should return an error if this returns false
// and the next function will complain about type mismatch,
// going up the callstack, until we hit a try or not
// the checker is the type number of the function which is checking
bool expect_type(sm_object *arg_n, unsigned int arg_num, unsigned short int arg_type,
                 unsigned short int checker) {
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

// Future symbol system will optimize this function
// so that any two symbols with the same name will be the same symbol
bool is_true(sm_object *obj) {
  if (obj->my_type == SM_SYMBOL_TYPE) {
    sm_symbol *sym = (sm_symbol *)obj;
    if (sym->name->size == 4 && strncmp(&(sym->name->content), "true", 4) == 0) {
      return true;
    }
  }
  return false;
}

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
    case SM_SLEEP_EXPR: {
      sm_object *obj0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      int        tms;
      if (expect_type(obj0, 0, SM_DOUBLE_TYPE, SM_SLEEP_EXPR))
        tms = (int)((sm_double *)obj0)->value;
      else
        return (sm_object *)sm_new_double(0);

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
      return (sm_object *)sm_new_symbol(sm_new_string(4, "true"));
      break;
    }
    case SM_LS_EXPR: {
      DIR               *dir;
      struct dirent     *entry;
      struct stat        file_stat;
      const unsigned int MAX_ENTRIES = 1000;
      sm_string         *entry_names[MAX_ENTRIES];
      bool               entry_types[MAX_ENTRIES];
      unsigned int       i = 0;

      char cwd[1024];
      if (getcwd(cwd, sizeof(cwd)) != NULL) {
      } else {
        printf("Error: Current working directory is invalid: %s .\n", cwd);
        return (sm_object *)sm_new_double(0);
      }

      dir = opendir(cwd);
      if (dir == NULL) {
        printf("Error: Current working directory is invalid: %s .\n", cwd);
        return (sm_object *)sm_new_double(0);
      }
      while ((entry = readdir(dir)) != NULL && i < MAX_ENTRIES) {
        unsigned int path_length = strlen(cwd) + strlen(entry->d_name) + 1;
        char         full_path[path_length + 1];
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
      for (unsigned int names_i = 0; names_i < i; names_i++) {
        sm_expr_set_arg(names_arr, names_i, (sm_object *)entry_names[names_i]);
        if (entry_types[names_i] != 0)
          sm_expr_set_arg(types_arr, names_i, (sm_object *)sm_new_symbol(sm_new_string(4, "true")));
        else
          sm_expr_set_arg(types_arr, names_i,
                          (sm_object *)sm_new_symbol(sm_new_string(5, "false")));
      }
      sm_expr *result =
        sm_new_expr_2(SM_ARRAY_EXPR, (sm_object *)names_arr, (sm_object *)types_arr);
      return (sm_object *)result;
      break;
    }
    case SM_PWD_EXPR: {
      char cwd[1024];
      if (getcwd(cwd, sizeof(cwd)) != NULL) {
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
      if (expect_type(obj0, 0, SM_STRING_TYPE, SM_CD_EXPR))
        path = (sm_string *)obj0;
      else
        return (sm_object *)sm_new_double(0);

      char *path_cstr = &path->content;
      if (chdir(path_cstr) == 0)
        return (sm_object *)sm_new_symbol(sm_new_string(4, "true"));
      else
        return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
      break;
    }
    case SM_DATE_STR_EXPR: {
      time_t     rawtime;
      struct tm *timeinfo;
      time(&rawtime);
      timeinfo          = localtime(&rawtime);
      sm_string *result = sm_new_string_manual(24);
      snprintf(&(result->content), 25, "%s", asctime(timeinfo));
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
      if (expect_type(obj0, 0, SM_STRING_TYPE, SM_STR_FIND_EXPR))
        haystack = (sm_string *)obj0;
      else
        return (sm_object *)sm_new_double(0);
      sm_object *obj1 = sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      sm_string *needle;
      if (expect_type(obj1, 1, SM_STRING_TYPE, SM_STR_FIND_EXPR))
        needle = (sm_string *)obj1;
      else
        return (sm_object *)sm_new_double(0);
      return (sm_object *)sm_str_find(haystack, needle);
      break;
    }
    case SM_STR_SPLIT_EXPR: {
      sm_object *obj0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_string *haystack;
      if (expect_type(obj0, 0, SM_STRING_TYPE, SM_STR_SPLIT_EXPR))
        haystack = (sm_string *)obj0;
      else
        return (sm_object *)sm_new_double(0);
      sm_object *obj1 = sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      sm_string *needle;
      if (expect_type(obj1, 1, SM_STRING_TYPE, SM_STR_SPLIT_EXPR))
        needle = (sm_string *)obj1;
      else
        return (sm_object *)sm_new_double(0);
      return (sm_object *)sm_str_split(haystack, needle);
      break;
    }
    case SM_STR_PART_EXPR: {
      sm_object *obj0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_string *str0;
      if (expect_type(obj0, 0, SM_STRING_TYPE, SM_STR_PART_EXPR))
        str0 = (sm_string *)obj0;
      else
        return (sm_object *)sm_new_string(0, "");
      sm_object *obj1 = sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      sm_double *start;
      if (expect_type(obj1, 1, SM_DOUBLE_TYPE, SM_STR_PART_EXPR))
        start = (sm_double *)obj1;
      else
        return (sm_object *)sm_new_string(0, "");
      sm_object *obj2 = sm_engine_eval(sm_expr_get_arg(sme, 2), current_cx, sf);
      sm_double *desired_len;
      if (expect_type(obj2, 2, SM_DOUBLE_TYPE, SM_STR_PART_EXPR))
        desired_len = (sm_double *)obj2;
      else
        return (sm_object *)sm_new_string(0, "");
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
      if (expect_type(obj0, 0, SM_STRING_TYPE, SM_STR_CAT_EXPR))
        str0 = (sm_string *)obj0;
      else
        return (sm_object *)sm_new_string(0, "");
      sm_object *obj1 = sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      sm_string *str1;
      if (expect_type(obj1, 1, SM_STRING_TYPE, SM_STR_CAT_EXPR))
        str1 = (sm_string *)obj1;
      else
        return (sm_object *)sm_new_string(0, "");
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
      if (expect_type(obj0, 0, SM_STRING_TYPE, SM_STR_SIZE_EXPR))
        str0 = (sm_string *)obj0;
      else
        return (sm_object *)sm_new_string(0, "");
      return (sm_object *)sm_new_double(str0->size);
      break;
    }
    case SM_EXIT_EXPR: {
      sm_object *obj0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_double *num0;
      if (expect_type(obj0, 0, SM_DOUBLE_TYPE, SM_EXIT_EXPR))
        num0 = (sm_double *)obj0;
      else
        return (sm_object *)sm_new_string(0, "");
      int exit_code = num0->value;
      sm_signal_exit(exit_code);
      break;
    }
    case SM_LET_EXPR: {
      sm_symbol *sym   = (sm_symbol *)sm_expr_get_arg(sme, 0);
      sm_object *value = (sm_object *)sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      if (expect_type((sm_object *)sym, 0, SM_SYMBOL_TYPE, SM_LET_EXPR)) {
        if (sm_cx_let(current_cx, &sym->name->content, sym->name->size, value))
          return (sm_object *)sm_new_symbol(sm_new_string(4, "true"));
      }
      return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
    }
    case SM_CX_LET_EXPR: {
      sm_cx     *cx    = (sm_cx *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_symbol *sym   = (sm_symbol *)sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      sm_object *value = (sm_object *)sm_engine_eval(sm_expr_get_arg(sme, 2), current_cx, sf);
      if (expect_type((sm_object *)cx, 0, SM_CX_TYPE, SM_CX_LET_EXPR)) {
        if (expect_type((sm_object *)sym, 1, SM_SYMBOL_TYPE, SM_CX_LET_EXPR)) {
          if (sm_cx_let(cx, &sym->name->content, sym->name->size, value))
            return (sm_object *)sm_new_symbol(sm_new_string(4, "true"));
        }
      }
      return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
    }
    case SM_CX_DOT_EXPR: {
      sm_cx     *cx  = (sm_cx *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_symbol *sym = (sm_symbol *)sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      if (expect_type((sm_object *)cx, 0, SM_CX_TYPE, SM_CX_DOT_EXPR)) {
        if (expect_type((sm_object *)sym, 1, SM_SYMBOL_TYPE, SM_CX_DOT_EXPR)) {
          sm_object *retrieved = sm_cx_get(cx, &sym->name->content, sym->name->size);
          if (retrieved != NULL)
            return retrieved;
          else
            return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
        }
      }
      return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
    }
    case SM_CX_CLEAR_EXPR: {
      sm_cx *cx = (sm_cx *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      if (expect_type((sm_object *)cx, 0, SM_CX_TYPE, SM_CX_CLEAR_EXPR)) {
        sm_cx_clear(cx);
        return (sm_object *)sm_new_symbol(sm_new_string(4, "true"));
      }
      return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
    }
    case SM_CX_CONTAINING_EXPR: {
      sm_cx     *cx  = (sm_cx *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_symbol *sym = (sm_symbol *)sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      if (expect_type((sm_object *)cx, 0, SM_CX_TYPE, SM_CX_CONTAINING_EXPR)) {
        if (expect_type((sm_object *)sym, 1, SM_SYMBOL_TYPE, SM_CX_CONTAINING_EXPR)) {
          sm_cx *retrieved = sm_cx_get_container(cx, &sym->name->content, sym->name->size);
          if (retrieved != NULL)
            return (sm_object *)retrieved;
          else
            return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
        }
      }
      return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
    }
    case SM_CX_SIZE_EXPR: {
      sm_cx *cx = (sm_cx *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      if (expect_type((sm_object *)cx, 0, SM_CX_TYPE, SM_CX_SIZE_EXPR)) {
        int size = sm_cx_size(cx);
        return (sm_object *)sm_new_double(size);
      }
      return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
    }
    case SM_RM_EXPR: {
      sm_symbol *sym = (sm_symbol *)sm_expr_get_arg(sme, 0);
      if (expect_type((sm_object *)sym, 0, SM_SYMBOL_TYPE, SM_RM_EXPR)) {
        bool success = sm_cx_rm(current_cx, &sym->name->content, sym->name->size);
        if (success == true)
          return ((sm_object *)sm_new_symbol(sm_new_string(4, "true")));
        else
          return ((sm_object *)sm_new_symbol(sm_new_string(5, "false")));
      }
      return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
    }
    case SM_CX_RM_EXPR: {
      sm_cx     *cx  = (sm_cx *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_symbol *sym = (sm_symbol *)sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      if (expect_type((sm_object *)cx, 0, SM_CX_TYPE, SM_CX_RM_EXPR)) {
        if (expect_type((sm_object *)sym, 1, SM_SYMBOL_TYPE, SM_CX_RM_EXPR)) {
          bool success = sm_cx_rm(cx, &sym->name->content, sym->name->size);
          if (success == true)
            return ((sm_object *)sm_new_symbol(sm_new_string(4, "true")));
          else
            return ((sm_object *)sm_new_symbol(sm_new_string(5, "false")));
        }
      }
      return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
    }
    case SM_CX_KEYS_EXPR: {
      sm_cx *cx = (sm_cx *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      if (expect_type((sm_object *)cx, 0, SM_CX_TYPE, SM_CX_KEYS_EXPR)) {
        sm_expr *success =
          sm_node_keys(cx->content, sm_new_stack(32), sm_new_expr_n(SM_ARRAY_EXPR, 0, 0));
        if (success != NULL)
          return (sm_object *)success;
        else
          return ((sm_object *)sm_new_symbol(sm_new_string(5, "false")));
      }
      return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
    }
    case SM_STR_ESCAPE_EXPR: {
      sm_object *obj0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_string *str0;
      if (expect_type(obj0, 0, SM_STRING_TYPE, SM_STR_ESCAPE_EXPR))
        str0 = (sm_string *)obj0;
      else
        return (sm_object *)sm_new_string(0, "");
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
      sm_object *obj1 = sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      if (obj0->my_type == SM_SYMBOL_TYPE) {
        if (is_true(obj0)) {
          return (sm_object *)sm_new_symbol(sm_new_string(4, "true"));
        }
      }
      if (obj1->my_type == SM_SYMBOL_TYPE) {
        if (is_true(obj1)) {
          return (sm_object *)sm_new_symbol(sm_new_string(4, "true"));
        }
      }
      return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
    }
    case SM_NOT_EXPR: {
      sm_object *obj0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      if (obj0->my_type == SM_SYMBOL_TYPE) {
        if (is_true(obj0) == false) {
          return (sm_object *)sm_new_symbol(sm_new_string(4, "true"));
        }
      }
      return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
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
    case SM_RANDOM_EXPR: {
      return (sm_object *)sm_new_double(((double)rand()) / ((double)RAND_MAX));
    }
    case SM_SEED_EXPR: {
      sm_object *obj0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_double *number;
      if (expect_type(obj0, 0, SM_DOUBLE_TYPE, SM_SEED_EXPR))
        number = (sm_double *)obj0;
      else
        return (sm_object *)sm_new_string(0, "");
      double val       = number->value;
      int    floor_val = val > 0 ? val + 0.5 : val - 0.5;
      srand((int)floor_val);
      return (sm_object *)sm_new_symbol(sm_new_string(5, "true"));
      ;
    }
    case SM_FILE_WRITE_EXPR: {
      // obtain the file name
      sm_string *fname_str;
      sm_object *evaluated_fname = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      if (expect_type(evaluated_fname, 0, SM_STRING_TYPE, SM_FILE_WRITE_EXPR))
        fname_str = (sm_string *)evaluated_fname;
      else
        return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
      char *fname_cstr = &(fname_str->content);

      // obtain the content to write
      sm_object *evaluated_content = sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      sm_string *content_str;
      if (expect_type(evaluated_content, 1, SM_STRING_TYPE, SM_FILE_WRITE_EXPR))
        content_str = (sm_string *)evaluated_content;
      else
        return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
      char *content_cstr = &(content_str->content);

      FILE *fptr = fopen(fname_cstr, "w");
      // check that file can be opened for writing
      if (fptr == NULL) {
        printf("fileWriteStr failed to open: %s\n", fname_cstr);
        return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
      }
      fprintf(fptr, "%s", content_cstr);
      fclose(fptr);
      return (sm_object *)sm_new_symbol(sm_new_string(4, "true"));
    }
    case SM_FILE_APPEND_EXPR: {
      // obtain the file name
      sm_string *fname_str;
      sm_object *evaluated_fname = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      if (expect_type(evaluated_fname, 0, SM_STRING_TYPE, SM_FILE_APPEND_EXPR))
        fname_str = (sm_string *)evaluated_fname;
      else
        return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
      char *fname_cstr = &(fname_str->content);

      // obtain the content to write
      sm_object *evaluated_content = sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      sm_string *content_str;
      if (expect_type(evaluated_content, 1, SM_STRING_TYPE, SM_FILE_WRITE_EXPR))
        content_str = (sm_string *)evaluated_content;
      else
        return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
      char *content_cstr = &(content_str->content);

      FILE *fptr = fopen(fname_cstr, "a");
      // check that file can be opened for writing
      if (fptr == NULL) {
        printf("fileAppendStr failed to open for appending: %s\n", fname_cstr);
        return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
      }
      fputs(content_cstr, fptr);
      fclose(fptr);
      return (sm_object *)sm_new_symbol(sm_new_string(4, "true"));
    }
    case SM_FILE_READ_EXPR: {
      sm_object *evaluated = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_string *fname_str;
      if (expect_type(evaluated, 0, SM_STRING_TYPE, SM_FILE_READ_EXPR))
        fname_str = (sm_string *)evaluated;
      else
        return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
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
      if (expect_type(evaluated, 0, SM_STRING_TYPE, SM_FILE_PART_EXPR))
        fname_str = (sm_string *)evaluated;
      else
        return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
      char *fname_cstr = &(fname_str->content);

      evaluated = sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      sm_double *start_pos;
      if (expect_type(evaluated, 1, SM_DOUBLE_TYPE, SM_FILE_PART_EXPR))
        start_pos = (sm_double *)evaluated;
      else
        return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));

      evaluated = sm_engine_eval(sm_expr_get_arg(sme, 2), current_cx, sf);
      sm_double *length;
      if (expect_type(evaluated, 2, SM_DOUBLE_TYPE, SM_FILE_PART_EXPR))
        length = (sm_double *)evaluated;
      else
        return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));


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
      if (expect_type(evaluated, 0, SM_STRING_TYPE, SM_FILE_EXISTS_EXPR))
        fname_str = (sm_string *)evaluated;
      else
        return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
      char *fname_cstr = &(fname_str->content);

      FILE *file = fopen(fname_cstr, "r");
      if (file == NULL) {
        return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
      }
      fclose(file);
      return (sm_object *)sm_new_symbol(sm_new_string(4, "true"));
    }

    case SM_FILE_RM_EXPR: {
      sm_object *evaluated = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_string *fname_str;
      if (expect_type(evaluated, 0, SM_STRING_TYPE, SM_FILE_RM_EXPR))
        fname_str = (sm_string *)evaluated;
      else
        return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
      char *fname_cstr = &(fname_str->content);

      int result = remove(fname_cstr);
      if (result != 0) {
        printf("fileRm failed: Could not rm file: %s\n", fname_cstr);
        return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
      }

      return (sm_object *)sm_new_symbol(sm_new_string(4, "true"));
    }
    case SM_FILE_STAT_EXPR: {
      sm_object *evaluated = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_string *fname_str;
      if (expect_type(evaluated, 0, SM_STRING_TYPE, SM_FILE_STAT_EXPR))
        fname_str = (sm_string *)evaluated;
      else
        return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
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
        sm_expr_set_arg(output, 10, (sm_object *)sm_new_double(filestat.st_atim.tv_sec));
        sm_expr_set_arg(output, 11, (sm_object *)sm_new_double(filestat.st_atim.tv_nsec));
        sm_expr_set_arg(output, 12, (sm_object *)sm_new_double(filestat.st_mtim.tv_sec));
        sm_expr_set_arg(output, 13, (sm_object *)sm_new_double(filestat.st_mtim.tv_nsec));
        sm_expr_set_arg(output, 14, (sm_object *)sm_new_double(filestat.st_ctim.tv_sec));
        sm_expr_set_arg(output, 15, (sm_object *)sm_new_double(filestat.st_ctim.tv_nsec));
      } else {
        printf("Failed to get file information.\n");
        return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
      }
      return (sm_object *)output;
    }
    case SM_FILE_PARSE_EXPR: {
      sm_object *evaluated = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_string *str;
      if (expect_type(evaluated, 0, SM_STRING_TYPE, SM_FILE_PARSE_EXPR))
        str = (sm_string *)evaluated;
      else {
        return (sm_object *)sm_new_double(0);
      }
      char           *cstr = &(str->content);
      sm_parse_result pr   = sm_parse_file(cstr);
      if (pr.return_val != 0) {
        printf("Error: Parser failed and returned %i.\n", pr.return_val);
        return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
      } else {
        sm_parse_done();
        return pr.parsed_object;
      }
      break;
    }
    case SM_PARSE_EXPR: {
      sm_object *evaluated = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_string *str;
      if (expect_type(evaluated, 0, SM_STRING_TYPE, SM_PARSE_EXPR))
        str = (sm_string *)evaluated;
      else {
        return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
      }
      char *cstr         = &(str->content);
      cstr[str->size]    = ';'; // Temporarily replacing the NULL char
      sm_parse_result pr = sm_parse_cstr(cstr, str->size + 1);
      cstr[str->size]    = '\0'; // Place the null char back
      if (pr.return_val != 0) {
        printf("Error: Parser failed and returned %i.\n", pr.return_val);
        return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
      } else
        return pr.parsed_object;
    }
    case SM_TO_STRING_EXPR: {
      sm_object *evaluated = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      return (sm_object *)sm_object_to_string(evaluated);
    }
    case SM_EVAL_EXPR: {
      sm_cx     *where_to_eval = current_cx;
      sm_object *evaluated     = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      return sm_engine_eval(evaluated, where_to_eval, sf);
    }
    case SM_CX_EVAL_EXPR: {
      sm_object *evaluated = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_object *obj1      = sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      sm_cx     *where_to_eval;
      if (expect_type(evaluated, 0, SM_CX_TYPE, SM_CX_EVAL_EXPR))
        where_to_eval = (sm_cx *)evaluated;
      else
        return (sm_object *)sm_new_double(0);
      return sm_engine_eval(obj1, where_to_eval, sf);
    }
    case SM_PUT_EXPR: {
      sm_object *evaluated = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_string *str;
      if (expect_type(evaluated, 0, SM_STRING_TYPE, SM_PUT_EXPR))
        str = (sm_string *)evaluated;
      else
        return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
      str = (sm_string *)evaluated;
      for (unsigned int i = 0; i < str->size; i++)
        putchar((&str->content)[i]);
      putchar('\0');
      fflush(stdout);
      return (sm_object *)sm_new_symbol(sm_new_string(4, "true"));
      break;
    }
    case SM_PUTLN_EXPR: {
      sm_object *evaluated = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_string *str;
      if (expect_type(evaluated, 0, SM_STRING_TYPE, SM_PUTLN_EXPR))
        str = (sm_string *)evaluated;
      else
        return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
      str = (sm_string *)evaluated;
      for (unsigned int i = 0; i < str->size; i++)
        putchar((&str->content)[i]);
      putchar('\n');
      putchar('\0');
      fflush(stdout);
      return (sm_object *)sm_new_symbol(sm_new_string(4, "true"));
      break;
    }
    case SM_WHILE_EXPR: {
      sm_expr   *condition  = (sm_expr *)sm_expr_get_arg(sme, 0);
      sm_object *expression = sm_expr_get_arg(sme, 1);
      while (is_true(sm_engine_eval((sm_object *)condition, current_cx, sf))) {
        // TODO: check for break/return here
        // if(eval==break || eval==return) break
        sm_engine_eval(expression, current_cx, sf);
      }
      return (sm_object *)sm_new_symbol(sm_new_string(4, "true"));
      break;
    }
    case SM_DO_WHILE_EXPR: {
      sm_expr   *condition  = (sm_expr *)sm_expr_get_arg(sme, 1);
      sm_object *expression = sm_expr_get_arg(sme, 0);
      do {
        sm_engine_eval(expression, current_cx, sf);
      } while (is_true(sm_engine_eval((sm_object *)condition, current_cx, sf)));
      return (sm_object *)sm_new_symbol(sm_new_string(4, "true"));
      break;
    }
    case SM_SIZE_EXPR: {
      sm_object *base_obj = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_expr   *arr;
      if (expect_type(base_obj, 0, SM_EXPR_TYPE, SM_SIZE_EXPR))
        arr = (sm_expr *)base_obj;
      else
        return (sm_object *)sm_new_double(-1);
      return (sm_object *)sm_new_double(arr->size);
    }
    case SM_MAP_EXPR: {
      // expecting a unary func
      sm_object *obj0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_fun    *fun;
      if (expect_type(obj0, 0, SM_FUN_TYPE, SM_MAP_EXPR))
        fun = (sm_fun *)obj0;
      else
        return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
      sm_object *obj1 = sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      sm_expr   *arr;
      if (expect_type(obj1, 1, SM_EXPR_TYPE, SM_MAP_EXPR))
        arr = (sm_expr *)obj1;
      else
        return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
      sm_expr *output = sm_new_expr_n(arr->op, arr->size, arr->size);
      for (unsigned int i = 0; i < arr->size; i++) {
        sm_object *current_obj = sm_expr_get_arg(arr, i);
        sm_expr   *new_sf      = sm_new_expr(SM_PARAM_LIST_EXPR, current_obj);
        sm_expr_set_arg(output, i, sm_engine_eval(fun->content, fun->parent, new_sf));
      }
      return (sm_object *)output;
    }
    case SM_INDEX_EXPR: {
      sm_object *base_obj = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_expr   *arr;
      if (expect_type(base_obj, 0, SM_EXPR_TYPE, SM_INDEX_EXPR))
        arr = (sm_expr *)base_obj;
      else
        return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
      sm_object *index_obj = sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      sm_double *index_double;
      if (expect_type(index_obj, 1, SM_DOUBLE_TYPE, SM_INDEX_EXPR))
        index_double = (sm_double *)index_obj;
      else
        return (sm_object *)sm_new_string(0, "");
      unsigned int index = (int)index_double->value;
      if (arr->size < index + 1 || index_double->value < 0) {
        printf("Error: Index out of range: %i . Array size is %i\n", index, arr->size);
        return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
      }
      return sm_expr_get_arg(arr, index);
    }
    case SM_DOT_EXPR: {
      sm_object *base_obj   = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_symbol *field_sym  = (sm_symbol *)sm_expr_get_arg(sme, 1);
      sm_string *field_name = field_sym->name;
      sm_cx     *base_cx;
      if (expect_type(base_obj, 0, SM_CX_TYPE, SM_DOT_EXPR))
        base_cx = (sm_cx *)base_obj;
      else
        return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
      sm_object *sr = sm_cx_get_far(base_cx, &field_name->content, field_name->size);
      if (sr == NULL) {
        sm_string *base_str = sm_object_to_string(base_obj);
        printf("Error: Could not find variable: %s within %s\n", &(field_name->content),
               &(base_str->content));
        return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
      }
      // return sm_cx_entries(sr.context)[sr.index].value;
      return (sm_object *)sr;
    }
    case SM_PARENT_EXPR: {
      sm_object *base_obj = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_cx     *base_cx;
      if (expect_type(base_obj, 0, SM_CX_TYPE, SM_PARENT_EXPR))
        base_cx = (sm_cx *)base_obj;
      else
        return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
      if (base_cx->parent == NULL) {
        return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
      }
      return (sm_object *)base_cx->parent;
    }
    case SM_DIFF_EXPR: {
      sm_object *evaluated0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_object *evaluated1 = sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      sm_symbol *sym1;
      if (expect_type(evaluated1, 0, SM_SYMBOL_TYPE, SM_DIFF_EXPR))
        sym1 = (sm_symbol *)evaluated1;
      else
        return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
      sm_object *result = sm_diff(evaluated0, sym1);
      result            = sm_simplify(result);
      return result;
    }
    case SM_SIMP_EXPR: {
      sm_object *evaluated0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      return sm_simplify(evaluated0);
    }
    case SM_FUN_CALL_EXPR: {
      struct sm_expr *args     = (struct sm_expr *)sm_expr_get_arg(sme, 1);
      sm_expr        *new_args = (sm_expr *)sm_engine_eval((sm_object *)args, current_cx, sf);
      sm_object      *obj0     = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, new_args);
      if (obj0->my_type == SM_FUN_TYPE) {
        sm_fun *fun = (sm_fun *)obj0;
        return sm_engine_eval(fun->content, fun->parent, new_args);
      } else if (obj0->my_type == SM_SYMBOL_TYPE) {
        sm_symbol *fun_sym = (sm_symbol *)obj0;
        sm_object *found =
          sm_cx_get_far(current_cx, &(fun_sym->name->content), fun_sym->name->size);
        if (found == NULL) {
          printf("Error: Function not found: %s\n", &(fun_sym->name->content));
          sm_string *err_msg =
            sm_new_string(27 + fun_sym->name->size, "Error: Function not found: ");
          sm_strncpy((&err_msg->content) + 27, &fun_sym->name->content, fun_sym->name->size);
          return (sm_object *)sm_new_error(err_msg, sm_new_string(1, "."), 0);
        }
        sm_fun *found_fun;
        if (expect_type(found, 0, SM_FUN_TYPE, SM_FUN_CALL_EXPR)) {
          found_fun = (sm_fun *)found;
          // TODO: Speed up idea ! problem is , this changes the toStr of this function call
          // so to use this speedup, we need to store 3 things in a function call:
          // 1: expression of the fn 2: the evaluated expression or false 3: the args
          // that way, we can keep the found function without changing the representation
          // sm_expr_set_arg(sme,0,found);
          return sm_engine_eval(found_fun->content, found_fun->parent, new_args);
        } else
          return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
      } else
        return obj0;
      break;
    }
    case SM_THEN_EXPR: {
      for (unsigned int i = 0; i + 1 < sme->size; i++)
        sm_engine_eval(sm_expr_get_arg(sme, i), current_cx, sf);
      return sm_engine_eval(sm_expr_get_arg(sme, sme->size - 1), current_cx, sf);
      break;
    }
    case SM_ASSIGN_EXPR: {
      sm_symbol *sym;
      sm_object *obj0  = sm_expr_get_arg(sme, 0);
      sm_object *value = (sm_object *)sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      if (expect_type(obj0, 0, SM_SYMBOL_TYPE, SM_ASSIGN_EXPR)) {
        sym = (sm_symbol *)obj0;
        if (sm_cx_set(current_cx, &sym->name->content, sym->name->size, value))
          return (sm_object *)sm_new_symbol(sm_new_string(4, "true"));
      }
      return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
    }
    case SM_ASSIGN_DOT_EXPR: {
      sm_object *obj0  = sm_expr_get_arg(sme, 0);
      sm_object *value = (sm_object *)sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      if (expect_type(obj0, 0, SM_EXPR_TYPE, SM_ASSIGN_DOT_EXPR)) {
        sm_expr *dot_expr = (sm_expr *)obj0;
        if (dot_expr->op != SM_DOT_EXPR) {
          char *error_msg = "Error: Incorrectly formatted AST.";
          int   len       = strlen(error_msg);
          printf("%s\n", error_msg);
          return (sm_object *)sm_new_error(sm_new_string(len, error_msg), sm_new_string(0, ""), 0);
        }
        sm_cx     *predot  = (sm_cx *)sm_engine_eval(sm_expr_get_arg(dot_expr, 0), current_cx, sf);
        sm_symbol *postdot = (sm_symbol *)sm_expr_get_arg(dot_expr, 1);
        if (expect_type((sm_object *)predot, 0, SM_CX_TYPE, SM_ASSIGN_DOT_EXPR)) {
          if (sm_cx_let(predot, &postdot->name->content, postdot->name->size, value))
            return (sm_object *)sm_new_symbol(sm_new_string(4, "true"));
        }
      }
      return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
    }
    case SM_ASSIGN_LOCAL_EXPR: {
      sm_object *obj0  = sm_expr_get_arg(sme, 0);
      sm_object *value = (sm_object *)sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      if (obj0->my_type == SM_LOCAL_TYPE) {
        sm_local *lcl = (sm_local *)obj0;
        sm_expr_set_arg(sf, lcl->index, value);
      } else
        return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
      return (sm_object *)sm_new_symbol(sm_new_string(4, "true"));
      break;
    }
    case SM_ASSIGN_INDEX_EXPR: {
      // expecting a[4]=value=> =index_expr(a,4,value);
      sm_object *obj2 = sm_engine_eval(sm_expr_get_arg(sme, 2), current_cx, sf);
      sm_object *obj0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_object *obj1 = sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      if (expect_type(obj0, 0, SM_EXPR_TYPE, SM_ASSIGN_INDEX_EXPR)) {
        sm_expr *arr = (sm_expr *)obj0;
        if (expect_type(obj1, 1, SM_DOUBLE_TYPE, SM_ASSIGN_INDEX_EXPR)) {
          sm_double *index = (sm_double *)obj1;
          if (index->value >= arr->size || index->value < -0) {
            printf("Error: Index out of range: %i\n", (int)index->value);
            return (sm_object *)sm_new_error(sm_new_string(19, "Index out of range"),
                                             sm_new_string(1, "?"), 0);
          }
          sm_expr_set_arg(arr, index->value, obj2);
          return (sm_object *)sm_new_symbol(sm_new_string(4, "true"));
        } else
          return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
      }
      return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
    }
    case SM_PLUS_EXPR: {
      double sum = 0;
      for (unsigned int i = 0; i < sme->size; i++) {
        sm_object *current_obj = sm_engine_eval(sm_expr_get_arg(sme, i), current_cx, sf);
        if (expect_type(current_obj, i, SM_DOUBLE_TYPE, sme->op))
          sum += ((sm_double *)current_obj)->value;
        else
          return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
      }
      return (sm_object *)sm_new_double(sum);
    }
    case SM_MINUS_EXPR: {
      sm_object *a = sm_expr_get_arg(sme, 0);
      a            = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      double sum   = ((sm_double *)a)->value;
      for (unsigned int i = 1; i < sme->size; i++) {
        sm_object *current_obj = sm_engine_eval(sm_expr_get_arg(sme, i), current_cx, sf);
        if (expect_type(current_obj, i, SM_DOUBLE_TYPE, sme->op))
          sum -= ((sm_double *)current_obj)->value;
        else
          return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
      }
      return (sm_object *)sm_new_double(sum);
    }
    case SM_TIMES_EXPR: {
      double product =
        ((sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf))->value;
      for (unsigned int i = 1; i < sme->size; i++) {
        sm_object *current_obj = sm_engine_eval(sm_expr_get_arg(sme, i), current_cx, sf);
        if (expect_type(current_obj, i, SM_DOUBLE_TYPE, sme->op))
          product *= ((sm_double *)current_obj)->value;
        else
          return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
      }
      return (sm_object *)sm_new_double(product);
    }
    case SM_DIVIDE_EXPR: {
      double quotient =
        ((sm_double *)sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf))->value;
      for (unsigned int i = 1; i < sme->size; i++) {
        sm_object *current_obj = sm_engine_eval(sm_expr_get_arg(sme, i), current_cx, sf);
        if (expect_type(current_obj, i, SM_DOUBLE_TYPE, SM_DIVIDE_EXPR))
          quotient /= ((sm_double *)current_obj)->value;
        else
          return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
      }
      return (sm_object *)sm_new_double(quotient);
    }
    case SM_POW_EXPR: {
      sm_object *arg0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_object *arg1 = sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      sm_double *num0;
      if (expect_type(arg0, 0, SM_DOUBLE_TYPE, SM_POW_EXPR)) {
        num0 = (sm_double *)arg0;
      } else
        return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
      sm_double *num1;
      if (expect_type(arg1, 1, SM_DOUBLE_TYPE, SM_POW_EXPR)) {
        num1 = (sm_double *)arg1;
      } else
        return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
      return (sm_object *)sm_new_double(pow(num0->value, num1->value));
    }
    case SM_SIN_EXPR: {
      sm_object *arg0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_double *num0;
      if (expect_type(arg0, 0, SM_DOUBLE_TYPE, SM_SIN_EXPR)) {
        num0 = (sm_double *)arg0;
      } else
        return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
      return (sm_object *)sm_new_double(sin(num0->value));
    }
    case SM_COS_EXPR: {
      sm_object *arg0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_double *num0;
      if (expect_type(arg0, 0, SM_DOUBLE_TYPE, SM_COS_EXPR)) {
        num0 = (sm_double *)arg0;
      } else
        return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
      return (sm_object *)sm_new_double(cos(num0->value));
    }
    case SM_TAN_EXPR: {
      sm_object *arg0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_double *num0;
      if (expect_type(arg0, 0, SM_DOUBLE_TYPE, SM_TAN_EXPR)) {
        num0 = (sm_double *)arg0;
      } else
        return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
      return (sm_object *)sm_new_double(tan(num0->value));
    }
    case SM_ASIN_EXPR: {
      // arcsin(x) = 2 * ln((1 + x^2 / (1 + sqrt(1 - x^2))^2) / sqrt(1 + x^4))
      sm_object *arg0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_double *num0;
      if (expect_type(arg0, 0, SM_DOUBLE_TYPE, SM_SIN_EXPR)) {
        num0 = (sm_double *)arg0;
      } else
        return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
      printf("asin is not implemented yet.\n");
      return (sm_object *)sm_new_double((num0->value));
    }
    case SM_ACOS_EXPR: {
      // acos(x) = pi/2 - ln((1 + ix / sqrt(1 - x^2)) / (1 - ix / sqrt(1 - x^2))) / 2i
      sm_object *arg0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_double *num0;
      if (expect_type(arg0, 0, SM_DOUBLE_TYPE, SM_COS_EXPR)) {
        num0 = (sm_double *)arg0;
      } else
        return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
      printf("acos is not implemented yet.\n");
      return (sm_object *)sm_new_double((num0->value));
    }
    case SM_ATAN_EXPR: {
      // arctan(x) = ln((1 + ix) / (1 - ix)) / 2i
      sm_object *arg0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_double *num0;
      if (expect_type(arg0, 0, SM_DOUBLE_TYPE, SM_TAN_EXPR)) {
        num0 = (sm_double *)arg0;
      } else
        return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
      printf("atan is not implemented yet.\n");
      return (sm_object *)sm_new_double((num0->value));
    }

    case SM_SEC_EXPR: {
      sm_object *arg0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_double *num0;
      if (expect_type(arg0, 0, SM_DOUBLE_TYPE, SM_SEC_EXPR)) {
        num0 = (sm_double *)arg0;
      } else
        return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
      return (sm_object *)sm_new_double(1.0 / cos(num0->value));
    }
    case SM_CSC_EXPR: {
      sm_object *arg0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_double *num0;
      if (expect_type(arg0, 0, SM_DOUBLE_TYPE, SM_CSC_EXPR)) {
        num0 = (sm_double *)arg0;
      } else
        return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
      return (sm_object *)sm_new_double(1.0 / sin(num0->value));
    }
    case SM_COT_EXPR: {
      sm_object *arg0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_double *num0;
      if (expect_type(arg0, 0, SM_DOUBLE_TYPE, SM_COT_EXPR)) {
        num0 = (sm_double *)arg0;
      } else
        return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
      return (sm_object *)sm_new_double(1.0 / tan(num0->value));
    }
    case SM_SINH_EXPR: {
      sm_object *arg0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_double *num0;
      if (expect_type(arg0, 0, SM_DOUBLE_TYPE, SM_SINH_EXPR)) {
        num0 = (sm_double *)arg0;
      } else
        return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
      return (sm_object *)sm_new_double(sinh(num0->value));
    }
    case SM_COSH_EXPR: {
      sm_object *arg0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_double *num0;
      if (expect_type(arg0, 0, SM_DOUBLE_TYPE, SM_COSH_EXPR)) {
        num0 = (sm_double *)arg0;
      } else
        return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
      return (sm_object *)sm_new_double(cosh(num0->value));
    }
    case SM_TANH_EXPR: {
      sm_object *arg0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_double *num0;
      if (expect_type(arg0, 0, SM_DOUBLE_TYPE, SM_TANH_EXPR)) {
        num0 = (sm_double *)arg0;
      } else
        return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
      return (sm_object *)sm_new_double(tanh(num0->value));
    }
    case SM_SECH_EXPR: {
      sm_object *arg0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_double *num0;
      if (expect_type(arg0, 0, SM_DOUBLE_TYPE, SM_TANH_EXPR)) {
        num0 = (sm_double *)arg0;
      } else
        return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
      return (sm_object *)sm_new_double(1.0 / cosh(num0->value));
    }
    case SM_CSCH_EXPR: {
      sm_object *arg0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_double *num0;
      if (expect_type(arg0, 0, SM_DOUBLE_TYPE, SM_TANH_EXPR)) {
        num0 = (sm_double *)arg0;
      } else
        return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
      return (sm_object *)sm_new_double(1.0 / sinh(num0->value));
    }
    case SM_COTH_EXPR: {
      sm_object *arg0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_double *num0;
      if (expect_type(arg0, 0, SM_DOUBLE_TYPE, SM_TANH_EXPR)) {
        num0 = (sm_double *)arg0;
      } else
        return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
      return (sm_object *)sm_new_double(1.0 / tanh(num0->value));
    }
    case SM_LN_EXPR: {
      sm_object *arg0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_double *num0;
      if (expect_type(arg0, 0, SM_DOUBLE_TYPE, SM_TANH_EXPR)) {
        num0 = (sm_double *)arg0;
      } else
        return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
      return (sm_object *)sm_new_double(log(num0->value));
    }
    case SM_LOG_EXPR: {
      sm_object *arg0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_object *arg1 = sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      sm_double *num0;
      if (expect_type(arg0, 0, SM_DOUBLE_TYPE, SM_LOG_EXPR)) {
        num0 = (sm_double *)arg0;
      } else
        return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
      sm_double *num1;
      if (expect_type(arg1, 1, SM_DOUBLE_TYPE, SM_LOG_EXPR)) {
        num1 = (sm_double *)arg1;
      } else
        return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
      return (sm_object *)sm_new_double(log(num1->value) / log(num0->value));
    }
    case SM_EXP_EXPR: {
      sm_object *arg0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_double *num0;
      if (expect_type(arg0, 0, SM_DOUBLE_TYPE, SM_TANH_EXPR)) {
        num0 = (sm_double *)arg0;
      } else
        return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
      return (sm_object *)sm_new_double(exp(num0->value));
    }
    case SM_SQRT_EXPR: {
      sm_object *arg0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_double *num0;
      if (expect_type(arg0, 0, SM_DOUBLE_TYPE, SM_TANH_EXPR)) {
        num0 = (sm_double *)arg0;
      } else
        return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
      return (sm_object *)sm_new_double(sqrt(num0->value));
    }
    case SM_ABS_EXPR: {
      sm_object *arg0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_double *num0;
      if (expect_type(arg0, 0, SM_DOUBLE_TYPE, SM_TANH_EXPR)) {
        num0 = (sm_double *)arg0;
      } else
        return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
      return (sm_object *)sm_new_double(num0->value < 0 ? -1 * num0->value : num0->value);
    }
    case SM_IF_EXPR: {
      sm_object *condition_result = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      if (is_true(condition_result)) {
        return sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      }
      return (sm_object *)(sm_object *)sm_new_symbol(sm_new_string(5, "false"));
    }
    case SM_IF_ELSE_EXPR: {
      sm_object *condition_result = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      if (condition_result->my_type != SM_SYMBOL_TYPE) {
        return sm_engine_eval(sm_expr_get_arg(sme, 2), current_cx, sf);
      }
      if (is_true(condition_result)) {
        return sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      }
      return sm_engine_eval(sm_expr_get_arg(sme, 2), current_cx, sf);
    }
    case SM_ARRAY_EXPR: {
      sm_expr *new_arr = sm_new_expr_n(SM_ARRAY_EXPR, sme->size, sme->size);
      for (unsigned int i = 0; i < sme->size; i++) {
        sm_object *new_val = sm_engine_eval(sm_expr_get_arg(sme, i), current_cx, sf);
        sm_expr_set_arg(new_arr, i, new_val);
      }
      return (sm_object *)new_arr;
    }
    case SM_PARAM_LIST_EXPR: {
      sm_expr *new_arr = sm_new_expr_n(SM_PARAM_LIST_EXPR, sme->size, sme->size);
      for (unsigned int i = 0; i < sme->size; i++) {
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
            return (sm_object *)sm_new_symbol(sm_new_string(4, "true"));
          else
            return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
        }
        return (sm_object *)(sm_object *)sm_new_symbol(sm_new_string(5, "false"));
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
            return (sm_object *)sm_new_symbol(sm_new_string(4, "true"));
          else
            return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
        }
      }
      return (sm_object *)(sm_object *)sm_new_symbol(sm_new_string(5, "false"));
    }
    case SM_EQEQ_EXPR: {
      sm_object *obj1 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      sm_object *obj2 = sm_engine_eval(sm_expr_get_arg(sme, 1), current_cx, sf);
      if (obj1 == obj2) {
        return (sm_object *)sm_new_symbol(sm_new_string(4, "true"));
      }

      if (sm_object_eq(obj1, obj2))
        return (sm_object *)sm_new_symbol(sm_new_string(4, "true"));
      else
        return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
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
        return (sm_object *)sm_new_symbol(sm_new_string(4, "true"));
      else
        return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
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
        return (sm_object *)sm_new_symbol(sm_new_string(4, "true"));
      else
        return (sm_object *)sm_new_symbol(sm_new_string(5, "false"));
    }
    case SM_RUNTIME_META_EXPR: {
      sm_object *obj0 = sm_engine_eval(sm_expr_get_arg(sme, 0), current_cx, sf);
      return (sm_object *)sm_new_meta(obj0, current_cx);
    }
    default:
      return input;
    } // End of switch inside expr case
  }   // End of expr case
  case SM_META_TYPE: {
    return ((sm_meta *)input)->address;
  }
  case SM_SELF_TYPE: {
    return (sm_object *)((sm_self *)input)->context;
  }
  case SM_PRIMITIVE_TYPE: {
    printf("Primitives are not developed yet\n");
    fflush(stdout);
    return input;
  }
  case SM_SYMBOL_TYPE: {
    sm_symbol *sym      = (sm_symbol *)input;
    sm_string *var_name = sym->name;
    sm_object *sr       = sm_cx_get_far(current_cx, &(var_name->content), var_name->size);
    if (sr != NULL)
      return sr;
    else {
      // should return error object
      printf("Could not find variable: %s\n", &(var_name->content));
      return (sm_object *)sm_new_double(0);
    }
  }
  case SM_LOCAL_TYPE: {
    sm_local *local = (sm_local *)input;
    return sm_expr_get_arg(sf, local->index);
  }
  case SM_ERROR_TYPE: {
    // TODO: Add callstack info here.
    return input;
  }
  default:
    return input;
  }
}
