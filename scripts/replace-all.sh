#backup your code before you do this
str='s/sm_engine_push_eval/sm_engine_push/g';
find ../src/ -name *.c -exec sed -i $str {} +
find ../src/ -name *.h -exec sed -i $str {} +
find ../src/ -name *.y -exec sed -i $str {} +
find ../src/ -name *.l -exec sed -i $str {} +
find ../sms_src/examples/ -name *.sms -exec sed -i $str {} +
