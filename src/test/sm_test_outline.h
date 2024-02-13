// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

// Hardcoded max of 50 chapters.
// Easy to upgrade if necessary.
typedef struct test_outline {
  char    test_zone_path[128];
  int32_t num_chapters;
  int32_t num_subchapters[50];
  char   *chapter_names[50];
} test_outline;

test_outline *parse_test_outline(char *filepath);