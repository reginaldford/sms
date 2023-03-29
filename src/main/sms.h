// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <unistd.h>

// These are the major object types of SMS
// Keep syncronized with function sm_object_type_str
enum sm_object_type {
  SM_DOUBLE_TYPE,    // 0
  SM_EXPR_TYPE,      // 1
  SM_PRIMITIVE_TYPE, // 2
  SM_STRING_TYPE,    // 3
  SM_SYMBOL_TYPE,    // 4
  SM_CONTEXT_TYPE,   // 5
  SM_POINTER_TYPE,   // 6
  SM_META_TYPE,      // 7
  SM_SPACE_TYPE,     // 8
  SM_FUN_TYPE,       // 9
  SM_FUN_PARAM_TYPE, // 10
  SM_LOCAL_TYPE,     // 11
  SM_ERROR_TYPE,     // 12
  SM_UNKNOWN_TYPE    // 13
};

// Useful macros
#define MIN(x, y) (x < y ? x : y)
#define MAX(x, y) (x > y ? x : y)
#define ARRAY_SIZE(x) (sizeof(x) / sizeof(*x))
#define DEBUG_HERE(note)                                                                           \
  printf("%s line %i %s : %s \n", __FILE__, __LINE__, __FUNCTION__, note);                         \
  fflush(stdout);

// Windows <signal.h> lacks these signals because it isn't even POSIX
#define SIGHUP 1
#define SIGQUIT 3

#include "sm_options.h"
#include "sm_stack.h"
#include "sm_string.h"
#include "sm_heap.h"
#include "sm_object.h"
#include "sm_symbol.h"
#include "sm_expr.h"
#include "sm_context.h"
#include "sm_pointer.h"
#include "sm_space.h"
#include "sm_global.h"
#include "sm_double.h"
#include "sm_gc.h"
#include "sm_terminal.h"
#include "sm_fun.h"
#include "sm_engine.h"
#include "sm_meta.h"
#include "sm_signal.h"
#include "sm_parse_result.h"
#include "sm_init.h"
#include "sm_local.h"
#include "sm_error.h"
#include "sm_diff.h"
#include "sm_simplify.h"
