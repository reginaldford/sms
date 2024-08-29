#include "../sms.h"

// Function to write a buffer to a file in chunks of specified size
sm_object *write_buffer_to_file(FILE *fptr, char *buffer, size_t buffer_size, size_t chunk_size,
                                sm_expr *sme) {
  size_t total_written = 0;
  while (total_written < buffer_size) {
    size_t to_write = chunk_size;
    if (total_written + chunk_size > buffer_size) {
      to_write = buffer_size - total_written;
    }
    size_t written = fwrite(buffer + total_written, 1, to_write, fptr);
    if (written != to_write) {
      sm_symbol *title   = sm_new_symbol("fileWriteError", strlen("fileWriteError"));
      sm_string *message = sm_new_string(strlen("fileWrite failed during write operation"),
                                         "fileWrite failed during write operation");
      return (sm_object *)sm_new_error_from_expr(title, message, sme, NULL);
    }
    total_written += written;
  }
  return (sm_object *)sms_true;
}
