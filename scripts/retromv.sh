# Use this to let git know that you moved a file after you moved it
# Use tab completion to find the file, then seperate teh file name from th path
# Expecting retromv <dstdir> <dstfile>
mv $1/$2 ./
git mv $2 $1
