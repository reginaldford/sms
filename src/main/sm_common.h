// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

/// Returns whether a character is a digit
bool sm_is_digit(char c);
/// Returns whether a character is a letter of the alphabet
bool sm_is_letter(char c);
/// If the letter is A-Z, returns the lowercase a-z
char sm_to_lowercase(char c);
/// Returns -1 if lowercase(c) is not found within the string "bkmgt"
/// @note If the character is found, returns the character index in the string
int sm_is_unit(char c);
/// Parse a c string into an integer specifying a number of bytes.
/// @note Example: "123" would be interpretted as 123 bytes
/// @note Example: "123k" would be interpretted as 123 * 1024 bytes
/// @note Example: "123m" would be interpretted as 123 * 1024 ^ 2 bytes
/// @note Example: "123g" would be interpretted as 123 * 1024 ^ 3 bytes
/// @note Example: "123t" would be interpretted as 123 * 1024 ^ 4 bytes
unsigned long long sm_bytelength_parse(char *str, int length);
/// Prints to stdout the number of bytes with a reasonable unit
void sm_print_fancy_bytelength(unsigned long long bytelength);
