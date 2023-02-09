// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"
#include "sm_test.h"

int num_chapters() { return 7; }

char *chapter_name(int chapter) {
  char *names[] = {"parser", "math", "context", "string", "expr", "mathops", "gc"};
  if (chapter < 0 || chapter >= num_chapters()) {
    printf("Invalid chapter: %i\n", chapter);
    exit(-1);
  }
  return names[chapter];
}

int num_subchapters(int chapter) {
  int values[] = {2, 2, 0, 0, 0, 0, 0};
  if (chapter >= 0 && chapter < num_chapters() - 1) {
    return values[chapter];
  } else {
    return 0;
  }
}

void test_intro(int chapter, int subchapter, int test, char *desc) {
  printf("Test: %i.%i.%i : %s ...", chapter, subchapter, test, desc);
}

int display_width() { return 50; }

// for text alignment
char *spaces(int len) {
  int         my_len = -1;
  static char str[50];
  if (my_len == -1) {
    for (int i = 0; i < 50; i++) {
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

// countes 2-digit values to help text alignment
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

// run an sms file with tests
int perform_test_subchapter(int chapter, int subchapter) {
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
    printf("Parsing: %s... ", buf);
    freopen(buf, "r", stdin);
    sm_parse_result pr = sm_parse();
    if (pr.return_val == 0) {
      printf("Successful.\n");
      // we expect a context with 'test' set to an array of arrays, each with 3 objects.
      if (pr.parsed_object->my_type != sm_context_type) {
        printf("Top level object is not a context. Aborting.\n");
        exit(-1);
      }

      sm_context                *test_env = (sm_context *)pr.parsed_object;
      sm_search_result_cascading src = sm_context_find_far(test_env, sm_new_string(5, "tests"));
      if (src.found == false) {
        printf("Top level context must contain a key 'tests' associated to a nested array.");
        printf("Aborting.\n");
        exit(-1);
      }
      if (sm_context_get(src.context, src.index)->my_type != sm_expr_type) {
        printf("Value under 'test' should be an array.\n");
        exit(-1);
      }
      sm_expr *test_list = (sm_expr *)sm_context_get(src.context, src.index);

      for (unsigned int i = 0; i < test_list->size; i++) {
        sm_object *obj = sm_expr_get_arg(test_list, i);
        if (obj->my_type != sm_expr_type) {
          printf("Top level array should only contain arrays. Aborting.\n");
          exit(-1);
        }
        sm_expr *test_triplet = (sm_expr *)obj;
        if (test_triplet->size < 2) {
          printf("Not enough elements: %i. Each test array needs at least 2 elements.\n",
                 test_triplet->size);
          exit(-1);
        }
        char *test_description = " - ";
        if (test_triplet->size > 2) {
          test_description = &(((sm_string *)sm_expr_get_arg(test_triplet, 2))->content);
        }
        test_intro(chapter, subchapter, i, test_description);
        sm_object *outcome      = sm_engine_eval(sm_expr_get_arg(test_triplet, 0), test_env);
        sm_string *outcome_str  = sm_object_to_string(outcome);
        sm_object *expected     = sm_expr_get_arg(test_triplet, 1);
        sm_string *expected_str = sm_object_to_string(expected);
        int        diff         = strcmp(&(expected_str->content), &(outcome_str->content));
        int        alignment    = get_alignment(chapter, subchapter, i);
        if (diff != 0) {
          printf("%sFailed.\nRegarding %i . %i . %i:\n",
                 spaces(display_width() - (strlen(test_description) + alignment)), chapter,
                 subchapter, i);
          printf("Left  side: %s\n", &(outcome_str->content));
          printf("Right side: %s\n", &(expected_str->content));
          return 1;
        } else {
          printf("%sPassed.\n", spaces(display_width() - (strlen(test_description) + alignment)));
        }
      }
    } else {
      printf("Failed.\n");
    }
  }
  return -1;
}

// run 1 test from sms file
int perform_test(int chapter, int subchapter, int test) {
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
    printf("Parsing: %s\n", buf);
    freopen(buf, "r", stdin);
    sm_parse_result pr = sm_parse();
    if (pr.return_val == 0) {
      // we expect an array of arrays, each with 3 objects.
      if (pr.parsed_object->my_type != sm_context_type) {
        printf("Top level object is not a context. Aborting.\n");
        exit(-1);
      }

      sm_context                *test_env = (sm_context *)pr.parsed_object;
      sm_search_result_cascading src = sm_context_find_far(test_env, sm_new_string(5, "tests"));
      if (src.found == false) {
        printf("Top level context must contain a key 'tests' associated to a nested array.");
        printf("Aborting.\n");
        exit(-1);
      }
      if (sm_context_get(src.context, src.index)->my_type != sm_expr_type) {
        printf("Value under 'test' should be an array.\n");
        exit(-1);
      }
      sm_expr   *test_list = (sm_expr *)sm_context_get(src.context, src.index);
      sm_object *obj; // 1 element of the user-provided array under 'test'
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
      if (test_triplet->size < 2) {
        printf("Not enough elements: %i. Each test array needs at least 2 elements.\n",
               test_triplet->size);
        exit(-1);
      }
      char *test_description = " - ";
      if (test_triplet->size > 2) {
        test_description = &(((sm_string *)sm_expr_get_arg(test_triplet, 2))->content);
      }
      test_intro(chapter, subchapter, test, test_description);
      sm_object *outcome      = sm_engine_eval(sm_expr_get_arg(test_triplet, 0), test_env);
      sm_string *outcome_str  = sm_object_to_string(outcome);
      sm_object *expected     = sm_expr_get_arg(test_triplet, 1);
      sm_string *expected_str = sm_object_to_string(expected);
      int        diff         = strcmp(&(expected_str->content), &(outcome_str->content));
      int        alignment    = get_alignment(chapter, subchapter, test);

      if (diff != 0) {
        printf("%sFailed: Test %i . %i . %i: failed.\n",
               spaces(display_width() - (strlen(test_description) + alignment)), chapter,
               subchapter, test);
        printf("Left  side: %s\n", &(outcome_str->content));
        printf("Right side: %s\n", &(expected_str->content));
        return 1;
      } else {
        printf("%sPassed.\n", spaces(display_width() - (strlen(test_description) + alignment)));
      }
    } else {
      printf("Failed.\n");
    }
  }
  return -1;
}

// Passing no arguments will run all tests.
// User can pass 3 integers to main: chapter, subchapter, and test number
// User can also pass 2 integers: chapter and subchapter
// User can also just specify a chapter
int main(int num_args, char **argv) {
  int chapter    = -1;
  int subchapter = -1;
  int test       = -1;
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
          printf("Testing Subchapter %i . %i\n", ch, sub_ch);
          perform_test_subchapter(ch, sub_ch);
        }
      }
    }
  } else if (subchapter == -1 && test == -1 && num_subchapters(chapter) > 0) {
    if (num_subchapters(chapter) > 0) {
      printf("Testing Chapter %i, which has %i subchapters ( from %i.0 to %i.%i )\n", chapter,
             num_subchapters(chapter), chapter, chapter, num_subchapters(chapter) - 1);
      for (int sub_ch = 0; sub_ch < num_subchapters(chapter); sub_ch++) {
        printf("Testing Subchapter %i . %i\n", chapter, sub_ch);
        perform_test_subchapter(chapter, sub_ch);
      }
    }
  } else if (test == -1) {
    printf("Testing Subchapter %i . %i\n", chapter, subchapter);
    perform_test_subchapter(chapter, subchapter);
  } else {
    printf("Running Test %i . %i . %i\n", chapter, subchapter, test);
    perform_test(chapter, subchapter, test);
  }
}
