//Null termination character excluded in size field.
//Zero size string still has NULL termination.
typedef struct sm_string {
  enum object_type my_type;
  int size;
  int space; //make string at least 16 bytes
  char content;
} sm_string;

char *sm_strncpy(char *dest,const char *src, unsigned int n);
sm_string *sm_new_string(int size, char *str);
sm_string *sm_concat_strings(sm_string* str1,sm_string* str2);
