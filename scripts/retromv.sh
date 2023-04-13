# Use this to let git know that you moved a file after you moved it
# Use tab completion to find the file, then seperate the file name from the path
# Expecting retromv <dstdir> <dstfile>
mv -v $1/$2 ./
git mv $2 $1
