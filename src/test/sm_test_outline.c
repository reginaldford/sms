// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../main/sms.h"
#include "sm_test_outline.h"

// All allocated memory is in the test outline
void graceful_exit(test_outline *to_free, int val) {
  free(to_free);
  exit(val);
}

// Read the file at filepath, parse and validate the structure.
// If validated, return the completed test_outline, else exit with -1
test_outline *parse_test_outline(char *filepath) {
  int           file_path_length = strlen(filepath);
  test_outline *result_outline   = malloc(sizeof(test_outline));
  result_outline->num_chapters   = 0;
  const char *last_slash         = strrchr(filepath, '/');
  if (!last_slash) {
    last_slash = filepath;
  }
  sm_strncpy(result_outline->test_zone_path, filepath, file_path_length);
  sm_env env;
  env.mem_flag   = false;
  env.quiet_mode = true;
  sm_init(&env, 0, NULL);
  printf("Parsing test outline file: %s ...\n", filepath);
  sm_parse_result pr = sm_parse_file(filepath);
  if (pr.return_val != 0) {
    printf("There was an issue parsing %s \n", filepath);
    graceful_exit(result_outline, -1);
  }
  sm_object *parsed_object = pr.parsed_object;
  if (parsed_object->my_type != SM_CX_TYPE) {
    printf("File: %s does not contain a context as top level object. Aborting.\n", filepath);
    graceful_exit(result_outline, -1);
  }
  sm_cx     *outline_cx   = (sm_cx *)parsed_object;
  sm_symbol *chapters_sym = sm_new_symbol("chapters", 8);

  sm_object *chapters_obj = sm_cx_get(outline_cx, chapters_sym);
  if (chapters_obj == NULL || chapters_obj->my_type != SM_EXPR_TYPE) {
    printf("Top level context in outline file must contain a key associating 'chapters' with an "
           "tuple of contexts.\n");
    graceful_exit(result_outline, -1);
  }
  sm_expr *chapters_tuple = (sm_expr *)chapters_obj;
  for (uint32_t i = 0; i < chapters_tuple->size; i++) {
    sm_object *current_obj = sm_expr_get_arg(chapters_tuple, i);
    if (current_obj->my_type != SM_CX_TYPE) {
      printf("Each element in the chapters tuple must be a context.\n");
      graceful_exit(result_outline, -1);
    }
    sm_cx     *current_ch_cx = (sm_cx *)current_obj;
    sm_symbol *name_sym      = sm_new_symbol("name", 4);
    sm_object *ch_name_obj   = sm_cx_get(current_ch_cx, name_sym);
    if (ch_name_obj->my_type != SM_STRING_TYPE) {
      printf("Each context in the chapters tuple must associate 'name' with a string for the name "
             "of the chapter.\n");
      graceful_exit(result_outline, -1);
    }
    // successfully collected name
    sm_string *ch_name_str         = (sm_string *)ch_name_obj;
    sm_symbol *num_subchapters_sym = sm_new_symbol("num_subchapters", 15);

    sm_object *num_subchapters_obj = sm_cx_get(current_ch_cx, num_subchapters_sym);
    if (num_subchapters_obj->my_type != SM_F64_TYPE) {
      printf("In addition to the chapter name, the context must associate 'num_subchapters' with a "
             "number.\n");
      graceful_exit(result_outline, -1);
    }
    // successfully collected number of subchapters
    f64 num_subchapters = ((sm_f64 *)num_subchapters_obj)->value;
    result_outline->num_chapters++;
    result_outline->num_subchapters[i] = num_subchapters;
    result_outline->chapter_names[i]   = &(ch_name_str->content);
  }
  // store the path of the outline file as the test zone path
  int path_length = (char *)last_slash - (char *)filepath;
  sm_strncpy(result_outline->test_zone_path, filepath, path_length);
  return result_outline;
}
