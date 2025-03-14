# Removes empty lines and counts the lines of code in all .c, .h, .l, and .y files under src
cd ..
echo "C only:"
cat src/**/*.c src/**/*.h src/**/**/*.c src/**/**/*.h | grep -E -v '^[[:space:]]*$'  | wc -l
echo "C, Bison, Flex, and SMS:"
cat src/**/*.c src/**/*.h src/**/**/*.c src/**/**/*.h src/**/**/*.y src/**/**/*.l sms_src/**/*.sms | grep -E -v '^[[:space:]]*$'  | wc -l
