#!/bin/bash
# copy to top of project dir, and run to reset files that only differ by unix permission

# Get a list of files whose permissions have changed since the last commit
files=$(git diff --name-only --diff-filter=M HEAD | xargs file | grep "text" | awk '{if ($NF !~ /ASCII/) {print $1}}')

# Restore the original permissions for each file
for file in $files; do
  git restore --staged -- $file
done
