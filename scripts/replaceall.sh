#backup your code before you do this
find ../src/ -name *.c -exec sed -i 's/to_replace/replacement/g' {} +
find ../src/ -name *.h -exec sed -i 's/to_replace/replacement/g' {} +
