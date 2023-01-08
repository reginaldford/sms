// This project is licensed under the BSD 2 clause license. See LICENSE.txt for more information.
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

enum sm_object_type {
  sm_double_type,
  sm_expression_type,
  sm_primitive_type,
  sm_string_type,
  sm_symbol_type,
  sm_context_type,
  sm_pointer_type,
  sm_key_value_type,
  sm_meta_type,
};

#define DEBUG_HERE(note)                                                                           \
  printf("%s line %i %s : %s \n", __FILE__, __LINE__, __FUNCTION__, note);                         \
  fflush(stdout);

#define MIN(x, y) (x < y ? x : y)
#define MAX(x, y) (x > y ? x : y)
#define ARRAY_SIZE(x) (sizeof(x) / sizeof(*x))

#include "sm_string.h"
#include "sm_object.h"
#include "sm_memory_heap.h"
#include "sm_pointer.h"
#include "sm_symbol.h"
#include "sm_commands.h"
#include "sm_context.h"
#include "sm_double.h"
#include "sm_expression.h"
#include "sm_gc.h"
#include "sm_global.h"
#include "sm_terminal.h"
#include "sm_key_value.h"
#include "sm_engine.h"
#include "sm_meta.h"
#include "sm_signal.h"
