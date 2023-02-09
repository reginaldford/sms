// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"
#include "sm_test.h"
#include "sm_test_outline.h"

#define DISPLAY_WIDTH 50

test_outline *global_test_outline(test_outline *replacement) {
  static test_outline *to;
  if (replacement != NULL) {
    test_outline *temp = to;
    to                 = replacement;
    return temp;
  }
  return to;
}

int num_chapters() { return global_test_outline(NULL)->num_chapters; }

char *chapter_name(int chapter) {
  char **names = global_test_outline(NULL)->chapter_names;
  if (chapter < 0 || chapter >= num_chapters()) {
    printf("Invalid chapter: %i\n", chapter);
    exit(-1);
  }
  return names[chapter];
}

int num_subchapters(int chapter) {
  int *values = global_test_outline(NULL)->num_subchapters;
  if (chapter >= 0 && chapter <= num_chapters() - 1) {
    return values[chapter];
  } else {
    return 0;
  }
}

void test_intro(int chapter, int subchapter, int test, char *desc) {
  printf("Test: %i.%i.%i : %s ...", chapter, subchapter, test, desc);
}

// for text alignment
char *spaces(int len) {
  int         my_len = -1;
  static char str[DISPLAY_WIDTH];
  if (my_len == -1) {
    for (int i = 0; i < DISPLAY_WIDTH; i++) {
      str[i] = ' ';
    }
    str[len] = '\0';
    return str;
  }
  str[my_len] = ' ';
  str[len]    = '\0';
  my_len      = len;
  return str;
}

// Counts 2-digit values among provided integers to help text alignment
int get_alignment(int chapter, int subchapter, int test) {
  int alignment = 0;
  if (chapter >= 10)
    alignment++;
  if (subchapter >= 10)
    alignment++;
  if (test >= 10)
    alignment++;
  return alignment;
}

// Returns 0 if pr contains a successfully pared context
// with an array assigned to 'test'
sm_context *check_parsed_object(sm_parse_result pr) {
  // we expect an array of arrays, each with 3 objects.
  if (pr.parsed_object->my_type != sm_context_type) {
    printf("Top level object is not a context. Aborting.\n");
    return NULL;
  }
  sm_context                *test_env = (sm_context *)pr.parsed_object;
  sm_search_result_cascading src      = sm_context_find_far(test_env, sm_new_string(5, "tests"));
  if (src.found == false) {
    printf("Top level context must contain a key 'tests' associated to a nested array.");
    printf("Aborting.\n");
    return NULL;
  }
  sm_object *found = sm_context_get(src.context, src.index);
  if (found->my_type != sm_expr_type) {
    printf("Value under 'test' should be an array.\n");
    return NULL;
  }
  return (sm_context *)test_env;
}

// returns 0 if there are no problems
int check_specific_test(sm_expr *test_list, int test) {
  sm_object *obj;
  if (test >= 0 && test < (int)test_list->size) {
    obj = sm_expr_get_arg(test_list, test);
  } else {
    printf("Error: No such test number: %i. Max valid test number is %i.\n", test,
           test_list->size - 1);
    exit(-1);
  }
  if (obj->my_type != sm_expr_type) {
    printf("Top level array should only contain arrays. Aborting.\n");
    exit(-1);
  }
  sm_expr *test_triplet = (sm_expr *)obj;
  if (test_triplet->size < 3) {
    printf("Not enough elements: %i. Each test array needs 3 elements.\n", test_triplet->size);
    exit(-1);
  }
  return 0;
}

int perform_specific_test(sm_context *test_env, sm_expr *test_list, int chapter, int subchapter,
                          int test) {
  if (check_specific_test(test_list, test) != 0) {
    printf("Something as wrong with the format of the test.\n");
    return -1;
  }

  sm_expr *test_triplet     = (sm_expr *)sm_expr_get_arg(test_list, test);
  char    *test_description = &(((sm_string *)sm_expr_get_arg(test_triplet, 2))->content);
  test_intro(chapter, subchapter, test, test_description);
  sm_object *outcome      = sm_engine_eval(sm_expr_get_arg(test_triplet, 0), test_env);
  sm_string *outcome_str  = sm_object_to_string(outcome);
  sm_object *expected     = sm_expr_get_arg(test_triplet, 1);
  sm_string *expected_str = sm_object_to_string(expected);
  int        diff         = strcmp(&(expected_str->content), &(outcome_str->content));
  int        alignment    = get_alignment(chapter, subchapter, test);
  if (diff != 0) {
    printf("%sFailed.\nRegarding %i.%i.%i:\n",
           spaces(DISPLAY_WIDTH - (strlen(test_description) + alignment)), chapter, subchapter,
           test);
    printf("Left  side: %s\n", &(outcome_str->content));
    printf("Right side: %s\n", &(expected_str->content));
    return 1;
  } else {
    printf("%sPassed.\n", spaces(DISPLAY_WIDTH - (strlen(test_description) + alignment)));
    return 0;
  }
}

// run an sms file with tests
// if int test is -1, will do all tests
// else, will run the specified test
int perform_test_subchapter(int chapter, int subchapter, int test) {
  int num_fails = 0;
  if (chapter < -1 || chapter >= num_chapters()) {
    printf("Test chapter: %i out of range.\n", chapter);
    printf("Valid test chapters are from 0 to %i.\n", num_chapters() - 1);
  } else if (subchapter < -1 || subchapter >= num_subchapters(chapter)) {
    printf("Subchapter: %i out of range (max acceptable value is: %i).\n", subchapter,
           num_subchapters(chapter) - 1);
  } else {
    sm_init();
    char buf[64];
    sprintf(buf, "../test_zone/%s/%i.sms", chapter_name(chapter), subchapter);
    printf("Parsing: %s... \n", buf);
    freopen(buf, "r", stdin);
    sm_parse_result pr = sm_parse();

    // exits the program if there is a problem
    sm_context *test_env = check_parsed_object(pr);
    if (test_env == NULL) {
      printf("Something was wrong with the format of the file.\n");
      return -1;
    }
    sm_search_result_cascading src       = sm_context_find_far(test_env, sm_new_string(5, "tests"));
    sm_expr                   *test_list = (sm_expr *)sm_context_get(src.context, src.index);
    if (test == -1) {
      for (unsigned int i = 0; i < test_list->size; i++) {
        num_fails += perform_specific_test(test_env, test_list, chapter, subchapter, i);
      }
    } else {
      num_fails += perform_specific_test(test_env, test_list, chapter, subchapter, test);
    }
  }
  return num_fails;
}

// Passing no arguments will run all tests.
// User can pass 3 integers to main: chapter, subchapter, and test number
// User can also pass 2 integers: chapter and subchapter
// User can also just specify a chapter
int main(int num_args, char **argv) {
  int chapter    = -1;
  int subchapter = -1;
  int test       = -1;
  int num_fails  = 0;
  // reading the outline file
  char *filepath = "../test_zone/outline.sms";
  global_test_outline(parse_test_outline(filepath));
  if (num_args == 2) {
    chapter = atoi(argv[1]);
  } else if (num_args == 3) {
    chapter    = atoi(argv[1]);
    subchapter = atoi(argv[2]);
  } else if (num_args == 4) {
    chapter    = atoi(argv[1]);
    subchapter = atoi(argv[2]);
    test       = atoi(argv[3]);
  } else if (num_args > 4) {
    printf("Too many arguments.\n Only pass integer(s) for the chapter and, optionally, a ( "
           "subchapter and, optionally, a test number ) .\n");
  }
  if (chapter < -1 || chapter >= num_chapters()) {
    printf("Specified chapter: %i is out of range. Must be from 0 to %i.\n", chapter,
           num_chapters() - 1);
    return -1;
  }
  if (subchapter < -1 || subchapter >= num_subchapters(chapter)) {
    printf("Specified subchapter: %i is out of range. Must be from 0 to %i.\n", subchapter,
           num_subchapters(chapter) - 1);
    return -1;
  }
  if (test < -1) {
    printf("Specified test number: %i is out of range. Must be at least 0.\n", test);
    return -1;
  }
  // passing -1 means to test all.
  if (chapter == -1) {
    printf("Testing all %i Chapters\n", num_chapters());
    for (int ch = 0; ch < num_chapters(); ch++) {
      if (num_subchapters(ch) > 0) {
        printf("Testing Chapter %i, which has %i subchapters ( from %i.0 to %i.%i )\n", ch,
               num_subchapters(ch), ch, ch, num_subchapters(ch) - 1);
        for (int sub_ch = 0; sub_ch < num_subchapters(ch); sub_ch++) {
          printf("Testing Subchapter %i.%i\n", ch, sub_ch);
          int test_result = perform_test_subchapter(ch, sub_ch, -1);
          if (test_result == -1) {
            printf("Testing Error.\n");
            exit(test_result);
          }
          num_fails += test_result;
        }
      }
    }
  } else if (subchapter == -1 && test == -1 && num_subchapters(chapter) > 0) {
    if (num_subchapters(chapter) > 0) {
      printf("Testing Chapter %i, which has %i subchapters ( from %i.0 to %i.%i )\n", chapter,
             num_subchapters(chapter), chapter, chapter, num_subchapters(chapter) - 1);
      for (int sub_ch = 0; sub_ch < num_subchapters(chapter); sub_ch++) {
        printf("Testing Subchapter %i.%i\n", chapter, sub_ch);
        int test_result = perform_test_subchapter(chapter, sub_ch, -1);
        if (test_result == -1) {
          printf("Testing Error.\n");
          exit(test_result);
        }
        num_fails += test_result;
      }
    }
  } else if (test == -1) {
    printf("Testing Subchapter %i.%i\n", chapter, subchapter);
    int test_result = perform_test_subchapter(chapter, subchapter, -1);
    if (test_result == -1) {
      printf("Testing Error.\n");
      exit(test_result);
    }
    num_fails += test_result;
  } else {
    printf("Running Test %i.%i.%i\n", chapter, subchapter, test);
    int test_result = perform_test_subchapter(chapter, subchapter, test);
    if (test_result == -1) {
      printf("Testing Error.\n");
      exit(test_result);
    }
    num_fails += test_result;
  }

  if (num_fails == 1) {
    printf("\n!! There was a failed test. Reported above.\n");
  } else if (num_fails > 1) {
    printf("\n!! There were %i failed tests. Reported above.\n", num_fails);
  } else {
    printf("\nAll tests passed!\n");
  }
  return num_fails;
}
