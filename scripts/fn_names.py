# Prints an sms-compatible array of strings with all global function names from sm_global.c
import re
import sys

input_file = "../src/main/sm_global.c"

# Regular expressions to match the start and end patterns
# _version should be the first expression in sm_global.c
start_pattern = re.compile(r'\{')
version_pattern = re.compile(r'"\_version"')
end_pattern = re.compile(r'\}')

def process_file(input_file):
    with open(input_file, 'r') as infile:
        sys.stdout.write('[')
        start_printing = False
        brace_count = 0
        found_version = False
        
        for line in infile:
            # Remove comments
            line = re.sub(r'//.*$', '', line)

            # Check for the start pattern
            if not start_printing:
                if start_pattern.search(line):
                    brace_count += line.count('{')
                    start_printing = True
            
            # Check for the "version" pattern
            if start_printing and not found_version:
                if version_pattern.search(line):
                    found_version = True
            
            if start_printing and found_version:
                brace_count += line.count('{')
                brace_count -= line.count('}')
                if brace_count == 0:
                    break
                sys.stdout.write(line)
        sys.stdout.write('];')

if __name__ == "__main__":
    process_file(input_file)

