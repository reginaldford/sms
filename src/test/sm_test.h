// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

void test_intro(int chapter, int subchapter, int test, char * desc);
void test_subchapter_intro(int chapter, int subchapter);
void test_chapter_intro(int chapter);
int perform_test(int chapter, int subchapter, int test);
int num_chapters();
int num_subchapters(int chapter);
char * chapter_name(int chapter);

// Chapters
// 0  : parser
// 1  : numbers
// 2  : context
// 3  : string
// 4  : expr
// 5  : mathops
// 6  : gc

