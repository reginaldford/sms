// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

/** Captures parser output, or error.
@field return_val Return integer from bison generated parser. 0 means no errors.
@field parsed_object The parsed object, or NULL if return_val is non-zero **/
typedef struct sm_parse_result {
  int32_t    return_val;
  sm_object *parsed_object;
} sm_parse_result;

/// Parse a c string with known length
sm_parse_result sm_parse_cstr(char *cstr, int32_t len);
/// Parse a file located at fname
sm_parse_result sm_parse_file(char *fname);
