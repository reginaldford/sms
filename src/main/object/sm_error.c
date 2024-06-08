// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"

/// Error object which has not been used correctly yet
/// @note source line info is to be stored in the AST and eventually passed to here
sm_error *sm_new_error(sm_string *title, sm_string *message, sm_string *source, uint32_t line) {
  sm_error *new_error = sm_malloc(sizeof(sm_error));
  new_error->my_type  = SM_ERROR_TYPE;
  new_error->title    = title;
  new_error->message  = message;
  new_error->source   = source;
  new_error->line     = line;
  // collect filepath and line
  return new_error;
}

/// If !fake, print the error type to a string buffer. Return the length regardlessly.
/// Error type is a symbol involving things like "...Invalid" and "...Blocked"
int sm_error_sprint(sm_error *self, char *buffer, bool fake) {
  if (!fake)
    sprintf(buffer, "(%s)", &self->title->content);
  return self->title->size + 2;
}
