// This project is licensed under the BSD 2 clause license. See LICENSE.txt for more information.

// The kernel tests have chapters and tests (no subchapters).
// User can specify the chapter and test number (2 integers)
// Program returns number of failed tests.

#include "../sms.h"
#include <ctype.h>
#include "chapter_0.h"
#include "chapter_1.h"

#define CHAPTER(f)\
  if(chosen_chapter==-1||chosen_chapter==current_chapter){\
    printf("Chapter %i : \n",current_chapter);\
    num_fails+=f;\
  }\
  current_chapter++;

int main(int num_args, char **argv) {
  int chosen_chapter = -1;
  int chosen_test    = -1;
  int current_chapter             = 0;
  int num_fails = 0;
  int total_tests=0;
  
  if (num_args > 1) {
    if (isalpha(*(argv[1]))) {
      if (strcmp(argv[1], "--help") == 0) {
        printf("Supplying no arguments will run all tests.\n");
        printf("Example: ./%s\n", argv[0]);
        printf("Optionally supply an integer to specify the chapter to run.\n");
        printf("Example: ./%s 1\n", argv[0]);
        printf("Optionally supply another integer to specify the test to run.\n");
        printf("Example: ./%s 1 0\n", argv[0]);
        return 0;
      }
      printf("First argument must be integer for chapter.\n");
      return -1;
    }
    chosen_chapter = atoi(argv[1]);
    if (num_args > 2) {
      if (isalpha(*(argv[1])!=0)) {
        printf("Second argument must be for test number.\n");
        return -1;
      }
      chosen_test = atoi(argv[2]);
    }
  }

  printf("SMS Kernel Tests\n\n");
  
  CHAPTER(chapter_0(chosen_test));
  CHAPTER(chapter_1(chosen_test));

  if(num_fails==0){
    printf("All tests passed!");
  }else{  
    printf("%i tests failed.\n",num_fails);
  }
  return num_fails;
}
