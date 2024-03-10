// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information
#include "sms.h"

// Returns whether a character is a digit
bool sm_is_digit(char c) {
  int number = (int)c;
  // ASCII char 0 through 9 is 48 through 57
  return number >= 48 && number <= 57;
}

// Returns whether a character is a letter of the alphabet
bool sm_is_letter(char c) {
  int number = (int)c;
  // ASCII char A-Z is [65-90] inclusively and a-z is [97-122]
  return (number >= 65 && number <= 90) || (number >= 97 && number <= 122);
}

// If the letter is A-Z, returns the lowercase a-z
char sm_to_lowercase(char c) {
  int number = (int)c;
  if (number >= 65 && number <= 90)
    return (char)(number + 32);
  return c;
}

// Returns -1 if lowercase(c) is not found within the string "bkmgt"
// Otherwise, returns the index of
int sm_is_unit(char c) {
  c                 = sm_to_lowercase(c);
  const char *units = "bkmgt"; // memory units
  for (int i = 0; i < 5; i++)
    if (units[i] == c)
      return i;
  return -1;
}

// Parse a c string into an integer specifying a number of bytes.
uint64_t sm_bytelength_parse(char *str, int length) {
  char buffer[length]; // 16 characters for the numeric value
  int  j = 0;          // j is index, skipping spaces
  for (int i = 0; i <= length; i++) {
    char current_char = str[i];
    if (current_char == 0) {
      buffer[j] = 0;
      // If no unit is given, assume megabytes (m)
      uint64_t l = atof(buffer) * 1024 * 1024;
      return (double)atof(buffer) * 1024 * 1024;
    }
    if (sm_is_digit(current_char) || current_char == '.') {
      buffer[j] = current_char;
      j++;
    } else if (sm_is_unit(current_char) != -1) {
      // 0 means b, 1 means k, etc.
      int unit          = sm_is_unit(current_char);
      int unit_in_bytes = pow(1024, unit);
      buffer[j]         = 0;
      j++;
      return unit_in_bytes * atof(buffer);
    } else if (current_char == ' ')
      continue;
    else
      continue;
  }
  return atof(buffer);
}

void sm_print_fancy_bytelength(uint64_t bytelength) {
  const uint64_t KB = 1024;
  if (bytelength < KB)
    printf("%.4gB", (double)bytelength);
  else if (bytelength < KB * KB)
    printf("%.4gKB", (double)bytelength / KB);
  else if (bytelength < (KB * KB * KB))
    printf("%.4gMB", (double)bytelength / (KB * KB));
  else if (bytelength < (KB * KB * KB * KB))
    printf("%.4gGB", (double)bytelength / (KB * KB * KB));
  else if (bytelength < (KB * KB * KB * KB * KB))
    printf("%.4gTB", (double)bytelength / (KB * KB * KB * KB * KB));
  else if (bytelength < (KB * KB * KB * KB * KB * KB))
    printf("%.4gEB", (double)bytelength / (KB * KB * KB * KB * KB * KB));
  else if (bytelength < (KB * KB * KB * KB * KB * KB * KB))
    printf("%.4gPB", (double)bytelength / (KB * KB * KB * KB * KB * KB * KB));
  else
    printf("lots");
}
