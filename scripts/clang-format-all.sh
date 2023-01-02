find ../src -type f -name '*.c' -o -name '*.h' -exec clang-format -i {} \;
find ../src/tests -type f -name '*.c' -o -name '*.h' -exec clang-format -i {} \;
