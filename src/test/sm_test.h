// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

void    test_intro(int32_t chapter, int32_t subchapter, int32_t test, char *desc);
void    test_subchapter_intro(int32_t chapter, int32_t subchapter);
void    test_chapter_intro(int32_t chapter);
int32_t perform_test(int32_t chapter, int32_t subchapter, int32_t test);
int32_t num_chapters();
int32_t num_subchapters(int32_t chapter);
char   *chapter_name(int32_t chapter);
