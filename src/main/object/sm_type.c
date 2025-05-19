// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"

/// Return the name of this type.
/// Must be synchronized with enum sm_object_type in sms.h
char *sm_type_name(uint32_t which) {
  static char *response[]   = {"ui8",   "ui64", "i64",  "f64",    "cx",    "err",   "xp",    "prim",
                               "str",   "sym",  "node", "ptr",    "meta",  "space", "fun",   "param",
                               "local", "link", "self", "return", "block", "stack", "array", "?"};
  uint16_t     numResponses = sizeof(response) / sizeof(response[0]);
  if (which >= numResponses)
    which = numResponses - 1;
  return response[which];
}

/// Return the length of the name of this type.
/// Must be synchronized with enum sm_object_type
uint32_t sm_type_name_len(uint32_t which) {
  static uint32_t response_len[] = {3, 3, 2, 4, 3, 3, 2, 4, 3, 4, 5,
                                    3, 5, 5, 4, 5, 4, 6, 5, 5, 5, 1};
  uint16_t        numResponses   = sizeof(response_len) / sizeof(response_len[0]);
  if (which >= numResponses)
    which = numResponses - 1;
  return response_len[which];
}
