// This project is licensed under the BSD 2 clause license. See LICENSE.txt for more information.

// The kernel tests have chapters and tests (no subchapters).
// User can specify the chapter and test number (2 integers)
// Program returns number of failed tests.

#include "../main/sms.h"
#include <ctype.h>
#include "chapter_0.h" // Just report the size of structures.
#include "chapter_1.h" // Generating many objects and then collecting the garbage.
#include "chapter_2.h" // Verify some expression printing mechanics.
#include "chapter_3.h" // Checks that contexts can scope and that memory is managed correctly
// #include "chapter_4.h" // Testing space array system, for deallocated space

#define CHAPTER(f)                                                                                 \
  if (chosen_chapter == -1 || chosen_chapter == current_chapter) {                                 \
    printf("Chapter %i : \n", current_chapter);                                                    \
    num_fails += f;                                                                                \
  }                                                                                                \
  current_chapter++;

int32_t main(int32_t num_args, char **argv) {
  int32_t chosen_chapter  = -1;
  int32_t chosen_test     = -1;
  int32_t current_chapter = 0;
  int32_t num_fails       = 0;

  if (num_args > 1) {
    // isalpha returns nonzero if the character is alpha
    // Assuming the relative path to outline.sms does not start with an integer
    if (sm_is_letter(*(argv[1])) == 0) {
      if (strcmp(argv[1], "--help") == 0) {
        printf("Supplying no arguments will run all tests.\n");
        printf("Example: ./%s\n", argv[0]);
        printf("Optionally supply an integer to specify the chapter to run.\n");
        printf("Example: ./%s 1\n", argv[0]);
        printf("Optionally supply another integer to specify the test to run.\n");
        printf("Example: ./%s 1 0\n", argv[0]);
        return 0;
      }
      printf("First argument must be integer for chapter. Provided: %s\n", argv[1]);
      return -1;
    }
    chosen_chapter = atoi(argv[1]);
    if (num_args > 2) {
      if (isalpha(*(argv[1]) != 0)) {
        printf("Second argument must be for test number.\n");
        return -1;
      }
      chosen_test = atoi(argv[2]);
    }
  }

  printf("SMS Kernel Tests\n\n");

  CHAPTER(chapter_0(chosen_test));
  CHAPTER(chapter_1(chosen_test));
  CHAPTER(chapter_2(chosen_test));
  CHAPTER(chapter_3(chosen_test));
  // CHAPTER(chapter_4(chosen_test));

  if (num_fails == 0) {
    printf("\nKernel tests passed!\n");
  } else {
    printf("\n%i tests failed.\n", num_fails);
  }
  return num_fails;
}
