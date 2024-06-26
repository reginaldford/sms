// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"

/// Error object
sm_error *sm_new_error_blank() {
  sm_error *new_error = sm_malloc(sizeof(sm_error));
  new_error->my_type  = SM_ERR_TYPE;
  return new_error;
}

/// Creates an error with both title and message, with defaults if necessary
sm_error *sm_new_error(int title_len, char *title_str, int message_len, char *message_str,
                       int sourceLen, char *source, uint32_t line) {
  sm_error *new_error = sm_new_error_blank();
  new_error->title =
    (title_len > 0) ? sm_new_string(title_len, title_str) : sm_new_string(10, "GenericErr");
  new_error->message =
    (message_len > 0) ? sm_new_string(message_len, message_str) : sm_new_string(0, "");
  sm_string *sourceStr;
  if (sourceLen)
    sourceStr = sm_new_string(sourceLen, source);
  else
    sourceStr = sm_new_string(10, "(terminal)");

  new_error->source = sourceStr;
  new_error->line   = line;
  return new_error;
}

/// If !fake, print the error type to a string buffer. Return the length regardlessly.
/// Error type is a symbol involving things like "...Invalid" and "...Blocked"
int sm_err_sprint(sm_error *self, char *buffer, bool fake) {
  if (!fake)
    sprintf(buffer, "<%s,%s>", &self->title->content, &self->message->content);
  return self->title->size + self->message->size + 3;
}
