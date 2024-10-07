// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"

extern sm_symbol      *sms_false; // Assuming sms_false is declared elsewhere.
extern struct sm_heap *sms_heap;  // Global heap

/** @brief Finds a file named `needle` in the current directory or in the directories listed in
 * SMS_PATH.
 *  @param needle The name of the file to search for.
 *  @return Pointer to `sm_object` containing the path if found, otherwise returns `sms_false`.
 */
sm_object *sm_path_find(sm_string *needle) {
  if (needle == NULL || needle->size == 0) {
    return (sm_object *)sms_false; // Invalid needle
  }

  char cwd[pathconf("/", _PC_PATH_MAX)];
  if (getcwd(cwd, sizeof(cwd)) == NULL) {
    return (sm_object *)sms_false; // Failed to get the current working directory
  }

  // Construct full path for the current directory.
  uint32_t path_size = strlen(cwd) + 1 + needle->size;
  char     path_buffer[path_size + 1];
  snprintf(path_buffer, path_size + 1, "%s/%s", cwd, &needle->content);

  struct stat stat_buffer;
  if (stat(path_buffer, &stat_buffer) == 0) {
    // File found in the current directory.
    return (sm_object *)sm_new_string_at(sms_heap, strlen(path_buffer), path_buffer);
  }

  // Get the SMS_PATH environment variable.
  char *sms_path = getenv("SMS_PATH");
  if (sms_path == NULL) {
    return (sm_object *)sms_false; // SMS_PATH not set
  }

  // Handle escaped colons in the SMS_PATH variable.
  size_t sms_path_len = strlen(sms_path);
  char  *parsed_path  = malloc(sms_path_len + 1);
  if (!parsed_path) {
    return (sm_object *)sms_false; // Memory allocation failure
  }

  char *src = sms_path;
  char *dst = parsed_path;

  while (*src != '\0') {
    if (*src == '\\' && *(src + 1) == ':') {
      *dst++ = ':'; // Handle escaped colon
      src += 2;     // Skip both '\' and ':'
    } else {
      *dst++ = *src++;
    }
  }
  *dst = '\0';

  // Tokenize the parsed path string.
  char *save_ptr  = NULL;
  char *directory = strtok_r(parsed_path, ":", &save_ptr);
  while (directory != NULL) {
    // Construct the full path.
    uint32_t dir_len        = strlen(directory);
    uint32_t needle_len     = needle->size;
    uint32_t full_path_size = dir_len + 1 + needle_len;
    char     full_path[full_path_size + 1];
    snprintf(full_path, full_path_size + 1, "%s/%s", directory, &needle->content);

    // Check if the file exists in this path.
    if (stat(full_path, &stat_buffer) == 0) {
      free(parsed_path);
      return (sm_object *)sm_new_string_at(sms_heap, strlen(full_path), full_path);
    }

    directory = strtok_r(NULL, ":", &save_ptr);
  }

  free(parsed_path);

  // File not found.
  return (sm_object *)sms_false;
}
