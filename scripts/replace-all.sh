#backup your code before you do this
str='s/XP_SYMSTR/XP_OPSYM/g';
find ../src/ -name *.c -exec sed -i $str {} +
find ../src/ -name *.h -exec sed -i $str {} +
find ../src/ -name *.y -exec sed -i $str {} +
find ../src/ -name *.l -exec sed -i $str {} +
