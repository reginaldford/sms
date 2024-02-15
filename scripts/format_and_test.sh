# Standard testing to maintain working-state on the repo

# We are using gnu make, not BSD make
if [ "$(uname)" = "FreeBSD" ] || [ "$(uname)" = "OpenBSD" ] || [ "$(uname)" = "NetBSD" ]; then
    MAKE=gmake
else
    MAKE=make  # Default to 'make' for other systems
fi

cd ..
$MAKE clean
cd scripts
./clang-format-all.sh
cd ..
$MAKE all && \
cd bin && \
./sms_kernel_test && ./sms_test && echo "All tests passed."
