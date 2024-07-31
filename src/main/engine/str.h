// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

/// Return the index of the substring to_find within str, or return -1
sm_double *sm_str_find(sm_string *str, sm_string *to_find);
/// Returns a tuple with all portions of str, split by the substing, needle.
/// @example strSplit("abc123","1") => [ "abc" , "1" , "23"]
sm_expr *sm_str_split(sm_string *str, sm_string *needle);
