// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"

sm_error *sm_new_error(sm_string *message, sm_string *source, unsigned int line) {
  sm_error *new_error = sm_malloc(sizeof(sm_error));
  new_error->my_type  = SM_ERROR_TYPE;
  new_error->message  = message;
  new_error->source   = source;
  new_error->line     = line;
  return new_error;
}

int sm_error_sprint(sm_error *self, char *buffer, bool fake) {
  int line_num_len = self->line == 0 ? 1 : log(self->line) / log(10);
  int length       = 10 + 6 + 2 + self->source->size + line_num_len + self->message->size;
  if (!fake) {
    snprintf(buffer, length, "Error at: %s line %i: %s\n", &self->source->content, self->line,
             &self->message->content);
  }
  return length;
}
