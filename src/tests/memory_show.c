// This project is licensed under the BSD 2 clause license. See LICENSE.txt for more information.

#include "../sms.h"

int main(int argc, char *argv[]) {
  char *file_name = "memory.dat";
  char  ch;
  FILE *fp;
  char *buffer = malloc(8000);

  if (argc > 1) {
    file_name = argv[1];
  }
  printf("Reading memory file: %s\n", file_name);

  fp = fopen(file_name, "r"); // read mode

  if (fp == NULL) {
    perror("Error while opening the file.\n");
    exit(EXIT_FAILURE);
  }

  printf("The contents of %s file are:\n", file_name);
  unsigned int buffer_ptr = 0;
  while ((ch = fgetc(fp)) != EOF) {
    buffer[buffer_ptr] = ch;
    buffer_ptr++;
  }

  // now the file is in the buffer
  unsigned int buffer_size = buffer_ptr;
  buffer_ptr               = 0;
  while (buffer_ptr < buffer_size) {
    sm_object *obj = (sm_object *)&(buffer[buffer_ptr]);
    printf("%i  ", obj->my_type);
    buffer_ptr += sm_sizeof(obj);
  }


  fclose(fp);
  return 0;
}
