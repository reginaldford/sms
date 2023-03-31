# Standard testing to maintain working-state on the repo
cd ..
make clean
cd scripts
./clang-format-all.sh
cd ..
make all
cd bin
./sms_kernel_test && ./sms_test && echo "PASSED! Seems OK for commit.";
