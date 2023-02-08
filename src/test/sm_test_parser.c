#include "../sms.h"
#include "sm_test.h"

int subchapter_1(int chapter, int subchapter) {
  test_subchapter_intro(chapter, subchapter, "parser");
  
  test_intro(chapter, subchapter, 1, "parsing numbers");

  sm_init();
  sm_parse_result pr = sm_parse();
  if(pr.return_val==0){
    printf("parse was success\n");
    // sm_engine_eval(pr.parsed_object,*(sm_global_lex_stack(NULL)->top));
  } else {
    printf("PARSE ERROR!\n");
    return 1;
  }
  return 0;  
}

int subchapter_2(int chapter, int subchapter) {
  test_subchapter_intro(chapter, subchapter, "parser");

  test_intro(chapter, subchapter, 1, "just testing the tester");
  return 0;
}

int run_subchapter(int chapter, int subchapter) {
  switch (subchapter) {
  case 0:
    return subchapter_1(chapter, subchapter);
  case 1:
    return subchapter_2(chapter, subchapter);
  }
  return 0;
}

int sm_test_parser(int chapter, int subchapter) {
  test_ch_intro(chapter, "parser");
  if (subchapter != -1 && subchapter < num_subchapters(chapter)) {
    return run_subchapter(chapter, subchapter);
  }
  return 0;
}
