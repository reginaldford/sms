// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

/// Returns the derivative of the given expression, or 0.
/// @param obj Should be an expression, else will automatically return 0.
/// @param wrt Stands for With Respect To. Should be a symbol.
sm_object *sm_diff(sm_object *obj, sm_symbol *wrt);
