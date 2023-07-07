// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../main/sms.h"
#include "sm_test.h"
#include "sm_test_outline.h"
#include <ctype.h>

extern int yylineno;
#define DISPLAY_WIDTH 65

// Global for the test outline structure
test_outline *global_test_outline(test_outline *replacement) {
  static test_outline *to;
  if (replacement != NULL) {
    test_outline *temp = to;
    to                 = replacement;
    return temp;
  }
  return to;
}

// To track how many tests are performed
int global_num_tests(int inc) {
  static int num = 0;
  num += inc;
  return num;
}

// Return the total number of chapters in this outline
int num_chapters() { return global_test_outline(NULL)->num_chapters; }

// Return the name of this chapter
char *chapter_name(int chapter) {
  char **names = global_test_outline(NULL)->chapter_names;
  if (chapter < 0 || chapter >= num_chapters()) {
    printf("Invalid chapter: %i\n", chapter);
    exit(-1);
  }
  return names[chapter];
}

// Return the number of subchapters of this chapter
int num_subchapters(int chapter) {
  int *values = global_test_outline(NULL)->num_subchapters;
  if (chapter >= 0 && chapter <= num_chapters() - 1) {
    return values[chapter];
  } else {
    return 0;
  }
}

// Announce which test is being performed
void test_intro(int chapter, int subchapter, int test, char *desc) {
  printf("Test: %i.%i.%i : %s ...", chapter, subchapter, test, desc);
}

// For text alignment
char *spaces(int len) {
  static char str[DISPLAY_WIDTH];
  for (int i = 0; i < len; i++) {
    str[i] = ' ';
  }
  str[len] = '\0';
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

// Returns 0 if pr contains a successfully paired context
// with an array assigned to 'test'
sm_cx *check_parsed_object(sm_parse_result pr) {
  // We expect an array of arrays, each with 3 objects.
  if (pr.parsed_object->my_type != SM_CX_TYPE) {
    printf("Top level object is not a context. Aborting.\n");
    return NULL;
  }
  sm_cx     *test_env = (sm_cx *)pr.parsed_object;
  sm_object *sr       = sm_cx_get(test_env, "tests", 5);
  if (sr == NULL) {
    printf("Top level context must contain a key 'tests' associated to a nested array.");
    printf("Aborting.\n");
    return NULL;
  }
  // sm_object *found = sm_cx_get(src.context,);
  if (sr->my_type != SM_EXPR_TYPE) {
    printf("Value under 'test' should be an array.\n");
    return NULL;
  }
  return (sm_cx *)test_env;
}

// Return 0 if there are no problems
int check_specific_test(sm_expr *test_list, int test) {
  sm_object *obj;
  if (test >= 0 && test < (int)test_list->size) {
    obj = sm_expr_get_arg(test_list, test);
  } else {
    printf("Error: No such test number: %i. Max valid test number is %i.\n", test,
           test_list->size - 1);
    exit(-1);
  }
  if (obj->my_type != SM_EXPR_TYPE) {
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

int perform_specific_test(sm_cx *test_env, sm_expr *test_list, int chapter, int subchapter,
                          int test) {
  if (check_specific_test(test_list, test) != 0) {
    printf("Something was wrong with the format of the test.\n");
    return -1;
  }
  sm_expr *test_triplet     = (sm_expr *)sm_expr_get_arg(test_list, test);
  char    *test_description = &(((sm_string *)sm_expr_get_arg(test_triplet, 2))->content);
  test_intro(chapter, subchapter, test, test_description);

  sm_object *outcome      = sm_engine_eval(sm_expr_get_arg(test_triplet, 0), test_env, NULL);
  sm_string *outcome_str  = sm_object_to_string(outcome);
  sm_object *expected     = sm_expr_get_arg(test_triplet, 1);
  sm_string *expected_str = sm_object_to_string(expected);
  int        diff         = strcmp(&(expected_str->content), &(outcome_str->content));
  int        alignment    = get_alignment(chapter, subchapter, test);
  if (diff != 0) {
    // -8 for exact display width
    printf("\n%sFailed.\n", spaces(DISPLAY_WIDTH - 8));
    printf("  Left  side (evaluated): %s\n", &(outcome_str->content));
    printf("  Right side (literal)  : %s\n\n", &(expected_str->content));
    return 1;
  } else {
    // 26 for exact display width
    printf("%sPassed.\n", spaces(DISPLAY_WIDTH - (strlen(test_description) + alignment + 26)));
    return 0;
  }
}

// Run an sms file with tests
// If test == -1, do all tests
// Else, run the specified test
int perform_test_subchapter(unsigned int chapter, unsigned int subchapter, int test,
                            char *test_zone_path) {
  int num_fails = 0;
  if (chapter >= num_chapters()) {
    printf("Test chapter: %i out of range.\n", chapter);
    printf("Valid test chapters are from 0 to %i.\n", num_chapters() - 1);
  } else if (subchapter >= num_subchapters(chapter)) {
    printf("Subchapter: %i out of range (max acceptable value is: %i).\n", subchapter,
           num_subchapters(chapter) - 1);
  } else {
    sm_env env;
    env.mem_flag = false;
    sm_init(&env, 0, NULL, true);
    char buf[64];
    int  len = 0;
    if (subchapter != 0)
      len =
        7 + strlen(test_zone_path) + strlen(chapter_name(chapter)) + 1 + log(subchapter) / log(10);
    else
      len = 7 + strlen(test_zone_path) + strlen(chapter_name(chapter)) + 1;

    snprintf(buf, len, "%s/%s/%i.sms", test_zone_path, chapter_name(chapter), subchapter);
    // If test_zone_path is empty string, then we need to remove the leading "/" from buf
    if (test_zone_path[0] == '\0') {
      for (int i = 0; buf[i] != '\0' && i <= 62; i++) {
        buf[i] = buf[i + 1];
      }
    }
    printf("Parsing: %s... \n", buf);
    yylineno           = 1;
    sm_parse_result pr = sm_parse_file(buf);
    if (pr.return_val != 0) {
      printf("Error parsing the subchapter. Parser returned %i\n", pr.return_val);
      return -1;
    }
    // exits the program if there is a problem
    sm_cx *test_env = check_parsed_object(pr);
    if (test_env == NULL) {
      printf("Something was wrong with the format of the file.\n");
      return -1;
    }
    sm_expr *test_list = (sm_expr *)sm_cx_get(test_env, "tests", 5);

    if (test == -1) {
      global_num_tests(test_list->size);
      for (unsigned int i = 0; i < test_list->size; i++) {
        num_fails += perform_specific_test(test_env, test_list, chapter, subchapter, i);
      }
    } else {
      global_num_tests(1);
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
  int arg_shift  = 0; // depends on whether the first arg is a filepath

  // If the first arg starts with a letter or period,
  // we assume first arg is a file path for the outline file
  char *filepath = "../test_zone/outline.sms";
  // isdigit returns nonzero if the character is a digit
  // Assuming the relative path to outline.sms does not start with an integer
  if (num_args > 1 && isdigit(*argv[1]) == 0) {
    filepath = argv[1];
    arg_shift++;
  }
  global_test_outline(parse_test_outline(filepath));

  if (num_args == 2 + arg_shift) {
    chapter = atoi(argv[1 + arg_shift]);
  } else if (num_args == 3 + arg_shift) {
    chapter    = atoi(argv[1 + arg_shift]);
    subchapter = atoi(argv[2 + arg_shift]);
  } else if (num_args == 4 + arg_shift) {
    chapter    = atoi(argv[1 + arg_shift]);
    subchapter = atoi(argv[2 + arg_shift]);
    test       = atoi(argv[3 + arg_shift]);
  } else if (num_args > 4 + arg_shift) {
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
    for (unsigned int ch = 0; ch < num_chapters(); ch++) {
      if (num_subchapters(ch) > 0) {
        printf("Testing Chapter %i, which has %i subchapters ( from %i.0 to %i.%i )\n", ch,
               num_subchapters(ch), ch, ch, num_subchapters(ch) - 1);
        for (unsigned int sub_ch = 0; sub_ch < num_subchapters(ch); sub_ch++) {
          printf("Testing Subchapter %i.%i\n", ch, sub_ch);
          int test_result =
            perform_test_subchapter(ch, sub_ch, -1, global_test_outline(NULL)->test_zone_path);
          if (test_result == -1) {
            printf("Testing Error.\n");
            exit(test_result);
          }
          num_fails += test_result;
          sm_garbage_collect();
        }
      }
    }
  } else if (subchapter == -1 && test == -1 && num_subchapters(chapter) > 0) {
    if (num_subchapters(chapter) > 0) {
      printf("Testing Chapter %i, which has %i subchapters ( from %i.0 to %i.%i )\n", chapter,
             num_subchapters(chapter), chapter, chapter, num_subchapters(chapter) - 1);
      for (int sub_ch = 0; sub_ch < num_subchapters(chapter); sub_ch++) {
        printf("Testing Subchapter %i.%i\n", chapter, sub_ch);
        int test_result =
          perform_test_subchapter(chapter, sub_ch, -1, global_test_outline(NULL)->test_zone_path);
        if (test_result == -1) {
          printf("Testing Error.\n");
          exit(test_result);
        }
        num_fails += test_result;
        sm_garbage_collect();
      }
    }
  } else if (test == -1) {
    printf("Testing Subchapter %i.%i\n", chapter, subchapter);
    int test_result =
      perform_test_subchapter(chapter, subchapter, -1, global_test_outline(NULL)->test_zone_path);
    if (test_result == -1) {
      printf("Testing Error.\n");
      exit(test_result);
    }
    num_fails += test_result;
  } else {
    printf("Running Test %i.%i.%i\n", chapter, subchapter, test);
    int test_result =
      perform_test_subchapter(chapter, subchapter, test, global_test_outline(NULL)->test_zone_path);
    if (test_result == -1) {
      printf("Testing Error.\n");
      exit(test_result);
    }
    num_fails += test_result;
  }

  if (num_fails == 1) {
    printf("\n!! There was 1 failed test reported above. %i tests total.\n", global_num_tests(0));
  } else if (num_fails > 1) {
    printf("\n!! There were %i failed tests reported above. %i tests total.\n", num_fails,
           global_num_tests(0));
  } else {
    printf("\nAll %i tests passed!\n", global_num_tests(0));
  }
  return num_fails;
}
