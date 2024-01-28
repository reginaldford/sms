#backup your code before you do this
str="s/short int/int16_t/g";
find ../src/ -name "*.c" -print -exec sed -i "$str" {} +
find ../src/ -name "*.h" -print -exec sed -i "$str" {} +
find ../src/ -name "*.y" -print -exec sed -i "$str" {} +
find ../src/ -name "*.l" -print -exec sed -i "$str" {} +
