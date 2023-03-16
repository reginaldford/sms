# Removes empty lines and counts the lines of code in all .c, .h, .l, and .y files under src
cd ..
make clean
cat src/**/*.c src/**/*.h src/**/*.y src/**/*.l | grep -E -v '^[[:space:]]*$'  | wc -l
