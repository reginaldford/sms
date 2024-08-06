#!/bin/bash
# Backup your code before you do this
str='s/global_type_name/type_name/g'
find ../src/ -name "*.c" -exec sed -i -e "$str" {} +
find ../src/ -name "*.h" -exec sed -i -e "$str" {} +
find ../src/ -name "*.y" -exec sed -i -e "$str" {} +
find ../src/ -name "*.l" -exec sed -i -e "$str" {} +
find ../sms_src/examples/ -name "*.sms" -exec sed -i -e "$str" {} +

