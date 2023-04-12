# Standard testing to maintain working-state on the repo
cd ..
make all
cd bin && \
./sms_kernel_test && ./sms_test && echo "All tests passed."
