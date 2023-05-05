# Save the keyphrase where you were last coding
keyphrase="YOU ARE HERE"
grep -n -C2 "$keyphrase" ../src/**/*.c
grep -n -C2  "$keyphrase" ../src/**/**/*.c
