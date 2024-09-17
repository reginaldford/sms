// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

/**@brief Initialize SMS interpreter
@param options Stores all user options
@param num_args The argument count from the main function
@param argv The arguments from the main function
*/
void sm_init(sm_env *options, int num_args, char **argv);
void sm_init_from_img(sm_env *env, int num_args, char **argv, const char *img_file_path);
