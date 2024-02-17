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
#include <termios.h>

// We use __builtin_popcount_ll in sm_node.c
#if defined(__x86_64__)
#include <x86intrin.h>
#endif

/// These are the major object types of SMS
/// @note Must remain syncronized with function sm_object_type_str
enum sm_object_type {
  /// 0: double precision floating point (sm_double.h)
  SM_DOUBLE_TYPE,
  /// 1: Expression (sm_expr.h)
  SM_EXPR_TYPE,
  /// 2: Primitive is not used yet
  SM_PRIMITIVE_TYPE,
  /// 3: Strings (sm_string.h)
  SM_STRING_TYPE,
  /// 4: Symbols (sm_symbol.h)
  SM_SYMBOL_TYPE,
  /// 5: Contexts (sm_cx.h)
  SM_CX_TYPE,
  /// 6: Nodes (sm_node.h)
  SM_NODE_TYPE,
  /// 7: Pointers (for gc) (sm_pointer.h)
  SM_POINTER_TYPE,
  /// 8: Meta objects (sm_meta.h)
  SM_META_TYPE,
  /// 9: Space objects (sm_space.h)
  SM_SPACE_TYPE,
  /// 10: Functions (sm_fun.h)
  SM_FUN_TYPE,
  /// 11: Function parameters (sm_fun.h)
  SM_FUN_PARAM_TYPE,
  /// 12: Local variable reference (sm_local.h)
  SM_LOCAL_TYPE,
  /// 13: Linked list objects (not used yet) (sm_link.h)
  SM_LINK_TYPE,
  /// 14: Error objects (not (really) used yet) (sm_error.h)
  SM_ERROR_TYPE,
  /// 15: Reference to current scope. (sm_self.h)
  SM_SELF_TYPE,
  /// 16: Acts as return statement. (sm_return.h)
  SM_RETURN_TYPE,
  /// 17: Bytecode block. (note used yet) (sm_return.h)
  SM_BC_BLOCK_TYPE,
  /// 18: Stack (sm_stack_obj.h)
  SM_STACK_OBJ_TYPE,
  /// 19: Unrecognized
  SM_UNKNOWN_TYPE
};

// Useful macros
#define MIN(x, y) (x < y ? x : y)
#define MAX(x, y) (x > y ? x : y)
#define DEBUG_HERE(note)                                                                           \
  printf("%s line %i %s : %s \n", __FILE__, __LINE__, __FUNCTION__, note);                         \
  fflush(stdout);

#include "sm_common.h"
#include "memory/sm_heap.h"
#include "sm_env.h"
#include "sm_signal.h"
#include "sm_init.h"
#include "object/sm_string.h"
#include "object/sm_object.h"
#include "object/sm_expr.h"
#include "sm_stack.h"
#include "object/sm_stack_obj.h"
#include "object/sm_node.h"
#include "object/sm_cx.h"
#include "parser/sm_parse_result.h"
#include "object/sm_double.h"
#include "object/sm_fun.h"
#include "object/sm_local.h"
#include "object/sm_symbol.h"
#include "object/sm_link.h"
#include "object/sm_meta.h"
#include "object/sm_self.h"
#include "object/sm_error.h"
#include "object/sm_bc_block.h"
#include "memory/sm_pointer.h"
#include "memory/sm_space.h"
#include "memory/sm_gc.h"
#include "sm_global.h"
#include "terminal/sm_terminal_output.h"
#include "engine/str.h"
#include "engine/sm_diff.h"
#include "engine/sm_simplify.h"
#include "object/sm_return.h"
#include "engine/sm_ast_engine.h"
#include "engine/op/stack.h"
