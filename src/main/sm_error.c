// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "sms.h"

sm_error *sm_new_error(sm_string *message, sm_string *source, unsigned int line) {
  sm_error *new_error = sm_malloc(sizeof(sm_error));
  new_error->my_type  = sm_error_type;
  new_error->message  = message;
  new_error->source   = source;
  new_error->line     = line;
  return new_error;
}

int sm_error_sprint(sm_error *self, char *buffer, bool fake) {
  if (!fake)
    sprintf(buffer, "Error at: %s line %i: %s\n", &self->source->content, self->line,
            &self->message->content);
  return 10 + 6 + 2 + self->source->size + sm_double_len(self->line) + self->message->size;
}