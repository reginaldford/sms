// The following file is provided under the BSD 2-clause license. For more info, read LICENSE.txt.

#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

enum sm_object_type {
  sm_double_type,
  sm_expr_type,
  sm_primitive_type,
  sm_string_type,
  sm_symbol_type,
  sm_context_type,
  sm_pointer_type,
  sm_key_value_type,
  sm_meta_type,
  sm_space_type
};

// Useful macros
#define MIN(x, y) (x < y ? x : y)
#define MAX(x, y) (x > y ? x : y)
#define ARRAY_SIZE(x) (sizeof(x) / sizeof(*x))
#define DEBUG_HERE(note)                                                                           \
  printf("%s line %i %s : %s \n", __FILE__, __LINE__, __FUNCTION__, note);                         \
  fflush(stdout);

// Windows lacks these signals
// in <signal.h> because it isn't even POSIX
#define SIGHUP 1
#define SIGQUIT 3

#include "sm_string.h"
#include "sm_heap.h"
#include "sm_object.h"
#include "sm_symbol.h"
#include "sm_context.h"
#include "sm_pointer.h"
#include "sm_commands.h"
#include "sm_space.h"
#include "sm_double.h"
#include "sm_expr.h"
#include "sm_gc.h"
#include "sm_global.h"
#include "sm_terminal.h"
#include "sm_key_value.h"
#include "sm_engine.h"
#include "sm_meta.h"
#include "sm_signal.h"
