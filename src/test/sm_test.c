#include "../sms.h"
#include "sm_test_context.h"
#include "sm_test_expr.h"
#include "sm_test_gc.h"
#include "sm_test_mathops.h"
#include "sm_test_numbers.h"
#include "sm_test_parser.h"
#include "sm_test_string.h"
#include "sm_test.h"

int num_chapters(){
  return 7;
}

int num_subchapters(int chapter){
  int values[] = {1, 1, 1, 1,1, 1, 1};
  if(chapter>=0 && chapter<num_chapters()-1){
    return values[chapter];
  }else{
    return 0;
  }
}


void test_ch_intro(int chapter, char *chapter_desc) {
  printf(">   Chapter %i : %s\n", chapter, chapter_desc);
}

void test_subchapter_intro(int chapter, int subchapter, char *sub_chapter_desc) {
  printf(">>  Subchapter: %i.%i : %s\n", chapter, subchapter, sub_chapter_desc);
}

void test_intro(int chapter, int subchapter, int test, char *sub_chapter_desc) {
  printf(">>> Test: %i.%i.%i : %s\n", chapter, subchapter, test, sub_chapter_desc);
}

int perform_test(int chapter, int subchapter) {
  if(chapter!=-1 && subchapter >= num_subchapters(chapter)){
    printf("Subchapter: %i out of range (max acceptable value is: %i).\n",subchapter,num_subchapters(chapter)-1);
  }
  switch (chapter) {
  // test the creation of objects from the parser
  case 0:
    return sm_test_parser(chapter,subchapter);
  // test the creation of objects from the parser
  case 1:
    return sm_test_numbers(subchapter);
  // test context functions
  case 2:
    return sm_test_context(subchapter);
  // test string functions
  case 3:
    return sm_test_string(subchapter);
  // test expr functions
  case 4:
    return sm_test_expr(subchapter);
  // test math functions
  case 5:
    return sm_test_mathops(subchapter);
  // test garbage collector
  case 6:
    return sm_test_gc(subchapter);

  default:
    printf("Test chapter: %i out of range.\n", chapter);
    printf("Valid test chapters are from 0 to %i.\n", num_chapters() - 1);
  }
  return -1;
}

int main(int num_args, char **argv) {

  int chapter    = -1;
  int subchapter = -1;
  if (num_args == 2) {
    chapter = atoi(argv[1]);
  } else if (num_args == 3) {
    chapter    = atoi(argv[1]);
    subchapter = atoi(argv[2]);
  } else if (num_args > 3) {
    printf("Too many arguments.\n Only pass chapter and, optionally, subchapter.\n (Both arguments "
           "are non-negative integers).");
  }

  // passing -1 means to test all chapters/subchapters.
  // selecting -1 for chapters automatically implies -1 for subchapter
  if (chapter == -1) {
    for (int ch = 0; ch < num_chapters(); ch++) {
      for (int sub_ch = 0; sub_ch < num_subchapters(ch); sub_ch++) {
        perform_test(ch, sub_ch);
      }
    }
  } else if (subchapter == -1) {
    for (int sub_ch = 0; sub_ch < num_subchapters(chapter); sub_ch++) {
      perform_test(chapter, sub_ch);
    }
  } else {
    perform_test(chapter, subchapter);
  }
}
