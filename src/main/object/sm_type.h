// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

/// Global names of types
char *sm_type_name(uint32_t which);
/// Global lengths of the type names
uint32_t sm_type_name_len(uint32_t which);
/// header size for objects of this type
uint8_t sm_type_header_size(uint32_t which);
