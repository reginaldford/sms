cd ..       &&
make clean  &&
cd scripts  &&
clang-format --verbose -i ../src/**/*.c ../src/**/*.h ../src/**/**/*.c ../src/**/**/*.h
