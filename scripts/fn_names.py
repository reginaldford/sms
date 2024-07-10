# Prints an sms-compatible array of strings with all global function names from sm_global.c
import re
import sys

input_file = "../src/main/sm_global.c"

# Regular expressions to match the start and end patterns
start_pattern = re.compile(r'\{')
exit_pattern = re.compile(r'"exit"')
end_pattern = re.compile(r'\}')

def process_file(input_file):
    with open(input_file, 'r') as infile:
        sys.stdout.write('[')
        start_printing = False
        brace_count = 0
        found_exit = False
        
        for line in infile:
            # Remove comments
            line = re.sub(r'//.*$', '', line)

            # Check for the start pattern
            if not start_printing:
                if start_pattern.search(line):
                    brace_count += line.count('{')
                    start_printing = True
            
            # Check for the "exit" pattern
            if start_printing and not found_exit:
                if exit_pattern.search(line):
                    found_exit = True
            
            if start_printing and found_exit:
                brace_count += line.count('{')
                brace_count -= line.count('}')
                if brace_count == 0:
                    break
                sys.stdout.write(line)
        sys.stdout.write('];')

if __name__ == "__main__":
    process_file(input_file)

