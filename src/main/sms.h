// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include <ctype.h>
#include <dirent.h>
#include <math.h>
#include <pwd.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <termios.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

#define f64 double
#define ui8 uint8_t

// We use __builtin_popcount_ll in sm_node.c
#if defined(__x86_64__)
#include <x86intrin.h>
#endif

// Set version number. Major.Minor.Patch
#define SMS_VERSION "0.21.46"
#define SMS_VERSION_LEN 7

/// These are the major object types of SMS
/// @note Must remain syncronized with function sm_type_name
enum sm_object_type {
  /// 0: f64 precision floating point (sm_f64.h)
  SM_F64_TYPE,
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
  /// 14: Error objects
  SM_ERR_TYPE,
  /// 15: Reference to current scope. (sm_self.h)
  SM_SELF_TYPE,
  /// 16: Acts as return statement. (sm_return.h)
  SM_RETURN_TYPE,
  /// 17: Bytecode block. (note used yet) (sm_return.h)
  SM_BC_BLOCK_TYPE,
  /// 18: Stack (sm_stack_obj.h)
  SM_STACK_OBJ_TYPE,
  /// 19: Array (sm_array.h)
  SM_ARRAY_TYPE,
  /// 20: unsigned byte int (sm_ui8.h)
  SM_UI8_TYPE,
  /// 21: Unrecognized
  SM_UNKNOWN_TYPE
};

// Useful macros
#define MIN(x, y) (x < y ? x : y)
#define MAX(x, y) (x > y ? x : y)
#define DEBUG(note)                                                                                \
  fprintf(stderr, "\n%s:%i %s : %s \n", __FILE__, __LINE__, __FUNCTION__, note);                   \
  fflush(stdout);
#include "linenoise/linenoise.h"
#include "sm_common.h"
#include "memory/sm_heap.h"
#include "sm_env.h"
#include "sm_signal.h"
#include "sm_init.h"
#include "object/sm_type.h"
#include "object/sm_string.h"
#include "object/sm_object.h"
#include "object/sm_expr.h"
#include "sm_stack.h"
#include "object/sm_stack_obj.h"
#include "object/sm_symbol.h"
#include "object/sm_node.h"
#include "object/sm_cx.h"
#include "parser/sm_parse_result.h"
#include "object/sm_f64.h"
#include "object/sm_ui8.h"
#include "object/sm_fun.h"
#include "object/sm_local.h"
#include "object/sm_meta.h"
#include "object/sm_self.h"
#include "object/sm_error.h"
#include "object/sm_bc_block.h"
#include "memory/sm_pointer.h"
#include "object/sm_space.h"
#include "object/sm_array.h"
#include "memory/sm_gc.h"
#include "sm_global.h"
#include "terminal/sm_terminal.h"
#include "engine/str.h"
#include "engine/sm_diff.h"
#include "engine/sm_simplify.h"
#include "object/sm_return.h"
#include "engine/sm_ast_engine.h"
#include "engine/op/stack.h"
#include "memory/sm_heap_set.h"
#include "memory/sm_img.h"
