#!/bin/bash

# Function to process a directory recursively
function process_directory {
  for file in "$1"/*; do
    if [[ -d "$file" ]]; then
      # If the file is a directory, call the function recursively
      process_directory "$file"
    elif [[ "$file" == *.sms ]]; then
      # If the file is a .sms file, replace "//" with "#" using sed
  		sed -i 's/\/\//#/g' "$file"
    fi
  done
}

# Call the function to process the starting directory
process_directory "./"
