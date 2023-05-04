// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <unistd.h>
#include <time.h>
#include <stdint.h>

// These are the major object types of SMS
// Keep syncronized with function sm_object_type_str
enum sm_object_type {
  SM_DOUBLE_TYPE,    // 0
  SM_EXPR_TYPE,      // 1
  SM_PRIMITIVE_TYPE, // 2
  SM_STRING_TYPE,    // 3
  SM_SYMBOL_TYPE,    // 4
  SM_CX_TYPE,        // 5
  SM_NODE_TYPE,      // 6
  SM_POINTER_TYPE,   // 7
  SM_META_TYPE,      // 8
  SM_SPACE_TYPE,     // 9
  SM_FUN_TYPE,       // 10
  SM_FUN_PARAM_TYPE, // 11
  SM_LOCAL_TYPE,     // 12
  SM_LINK_TYPE,      // 13
  SM_ERROR_TYPE,     // 14
  SM_UNKNOWN_TYPE    // 15
};

// Useful macros
#define ABS(x) (x < 0 ? -1 * x : x)
#define MIN(x, y) (x < y ? x : y)
#define MAX(x, y) (x > y ? x : y)
#define ARRAY_SIZE(x) (sizeof(x) / sizeof(*x))
#define DEBUG_HERE(note)                                                                           \
  printf("%s line %i %s : %s \n", __FILE__, __LINE__, __FUNCTION__, note);                         \
  fflush(stdout);

#include "sm_env.h"
#include "sm_signal.h"
#include "object/sm_string.h"
#include "object/sm_object.h"
#include "parser/sm_parse_result.h"
#include "sm_init.h"
#include "sm_stack.h"
#include "memory/sm_heap.h"
#include "object/sm_double.h"
#include "object/sm_fun.h"
#include "object/sm_local.h"
#include "object/sm_symbol.h"
#include "object/sm_expr.h"
#include "object/sm_link.h"
#include "object/sm_node.h"
#include "object/sm_cx.h"
#include "object/sm_meta.h"
#include "object/sm_error.h"
#include "memory/sm_pointer.h"
#include "memory/sm_space.h"
#include "memory/sm_gc.h"
#include "sm_global.h"
#include "sm_terminal.h"
#include "engine/str.h"
#include "engine/sm_engine.h"
#include "engine/sm_diff.h"
#include "engine/sm_simplify.h"
