#backup your code before you do this
find ../src/ -name *.c -exec sed -i 's/ugly/pretty/g' {} +
find ../src/ -name *.h -exec sed -i 's/ugly/pretty/g' {} +
find ../src/ -name *.l -exec sed -i 's/ugly/pretty/g' {} +
find ../src/ -name *.y -exec sed -i 's/ugly/pretty/g' {} +
