// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"

/// Return the name of this type.
/// Must be synchronized with enum sm_object_type in sms.h
char *sm_type_name(uint32_t which) {
  static char *response[] = {"f64",
                             "xp",
                             "prim",
                             "str",
                             "sym",
                             "cx",
                             "node",
                             "ptr",
                             "meta",
                             "space",
                             "fun",
                             "param",
                             "local",
                             "link",
                             "error",
                             "self",
                             "return",
                             "block",
                             "stack",
                             "array"
                             "ui8", // tmp fix for bug in engine's new_f64_expr case
                             "ui8",
                             "?"};
  if (which >= sizeof(response) / sizeof(response[0]))
    which = sizeof(response) / sizeof(response[0]) - 1;
  return response[which];
}

/// Return the length of the name of this type.
/// Must be synchronized with enum sm_object_type
uint32_t sm_type_name_len(uint32_t which) {
  static uint32_t response_len[] = {3, 2, 4, 3, 3, 2, 4, 3, 4, 5, 3,
                                    5, 5, 4, 5, 4, 6, 5, 5, 5, 3, 1};
  if (which >= sizeof(response_len) / sizeof(response_len[0]))
    which = sizeof(response_len) / sizeof(response_len[0]) - 1;
  return response_len[which];
}
