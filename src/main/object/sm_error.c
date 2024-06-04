// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"

/// Error object which has not been used correctly yet
/// @note source line info is to be stored in the AST and eventually passed to here
sm_error *sm_new_error(sm_string *message, sm_string *source) {
  sm_error *new_error = sm_malloc(sizeof(sm_error));
  new_error->my_type  = SM_ERROR_TYPE;
  new_error->message  = message;
  new_error->source   = source;
  return new_error;
}

/// If !fake, print the error to a string buffer. Return the length regardlessly.
int sm_error_sprint(sm_error *self, char *buffer, bool fake) {
  int length = 14 + self->source->size + self->message->size;
  if (!fake) {
    snprintf(buffer, length, "Error at: %s : %s\n", &self->source->content,
             &self->message->content);
  }
  return length;
}
