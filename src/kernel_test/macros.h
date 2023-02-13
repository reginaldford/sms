#define TEST(f) if(test==-1||test==current_test){\
    printf("%i: ",current_test);\
    f;\
  }\
  current_test++;

