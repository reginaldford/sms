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
    (title_len > 0) ? sm_new_symbol(title_str, title_len) : sm_new_symbol("GenericErr", 10);
  new_error->message =
    (message_len > 0) ? sm_new_string(message_len, message_str) : sm_new_string(0, "");
  sm_string *sourceStr;
  if (sourceLen)
    sourceStr = sm_new_string(sourceLen, source);
  else
    sourceStr = sm_new_string(7, "(stdin)");
  new_error->source = sourceStr;
  new_error->line   = line;
  // Run the error handler if it exists
  sm_cx   *scratch = *sm_global_lex_stack(NULL)->top;
  sm_fun  *fun     = (sm_fun *)sm_cx_get_far(scratch, sm_new_symbol("_errHandler", 11));
  sm_expr *sf      = sm_new_expr(SM_PARAM_LIST_EXPR, (sm_object *)new_error, NULL);
  if (fun)
    return (sm_error *)execute_fun(fun, sm_new_cx(NULL), sf);
  else
    return new_error;
}


/// Creates an error with both title and message, with defaults if necessary
sm_error *sm_new_error_from_strings(sm_symbol *title, sm_string *message, sm_string *source,
                                    uint32_t line, sm_cx *notes) {
  sm_error *new_error = sm_new_error_blank();
  new_error->title    = title;
  new_error->message  = message;
  new_error->source   = source;
  new_error->line     = line;
  new_error->notes    = notes;
  return new_error;
}
/// Creates an error with line and source from the expression
sm_error *sm_new_error_from_expr(sm_symbol *title, sm_string *message, sm_expr *sme, sm_cx *notes) {
  sm_string *source = (sm_string *)sm_cx_get(sme->notes, sm_new_symbol("source", 6));
  sm_f64    *line   = (sm_f64 *)sm_cx_get(sme->notes, sm_new_symbol("line", 4));
  sm_error  *e      = sm_new_error_from_strings(title, message, source, (int)line->value, notes);
  return sm_engine_eval(e, NULL, NULL);
}

/// If !fake, print the error type to a string buffer. Return the length regardlessly.
/// Error type is a symbol involving things like "...Invalid" and "...Blocked"
int sm_err_sprint(sm_error *self, char *buffer, bool fake) {
  if (self->message->size) {
    if (!fake)
      sprintf(buffer, "<%s,\"%s\">", &self->title->name->content, &self->message->content);
    return self->title->name->size + self->message->size + 5;
  }
  if (!fake)
    sprintf(buffer, "<%s>", &self->title->name->content);
  return self->title->name->size + self->message->size + 2;
}
