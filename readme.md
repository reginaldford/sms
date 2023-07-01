![GitHub](https://img.shields.io/github/license/reginaldford/sms)
![Lines of code](https://img.shields.io/tokei/lines/github/reginaldford/sms)
![GitHub top language](https://img.shields.io/github/languages/top/reginaldford/sms) 
![GitHub language count](https://img.shields.io/github/languages/count/reginaldford/sms)
![GitHub all releases](https://img.shields.io/github/downloads/reginaldford/sms/total)
![GitHub contributors](https://img.shields.io/github/contributors/reginaldford/sms?color=green)
![GitHub commit activity](https://img.shields.io/github/commit-activity/m/reginaldford/sms)
![img](https://img.shields.io/badge/maturity-alpha-orange)
![GitHub repo size](https://img.shields.io/github/repo-size/reginaldford/sms)
![GitHub Workflow Status](https://img.shields.io/github/actions/workflow/status/reginaldford/sms/make-check.yml)


# Introduction:
SMS (Symbolic Math System) is a math-focused scripting language for POSIX friendly operating systems. SMS has built-in functions for arithmetic, trigonometry, algebra, calculus, file access and more. SMS aims to become a simple general purpose programming language with a focus on mathematics (like Julia). SMS is still in early stages of development. SMS can be downloaded and installed from the Releases page on GitHub. The latest source code may have undocumented changes and this document is synchronized with the project at the point of the latest release: ![GitHub latest release (latest by date)](https://img.shields.io/github/v/tag/reginaldford/sms)

# Details

In addition to providing a command line, SMS can interpret files.
Run `sms -h` for command line options.

Unlike Javascript, Math functions in SMS are global keywords.

For example, taking a square root in javascript may look like `Math.sqrt(x)`. In SMS, this same function is permanently available as  `sqrt(x)`.

Another major difference between SMS and javascript is that SMS provides the meta operator (`:`) which allows developers to capture expressions for metaprogramming.

For example, taking a derivative of `sin(x)` with respect to `x` may be done with `diff(:sin(x),:x);` where the colon prevents SMS from evaluating the input and ultimately preserving the symbolic expression.

The following is 'hello world' in SMS:

`putln("Hello world!");`

SMS versions have 2 significant figures (like 0.180) during a milestone point. During these versions, development focuses on documentation, unit tests, and bugs.

SMS versions with 3 significant figures (like 0.181) are in feature development, so they may lack documentation for new features, and might have unannounced changes to existing features, are are generally less stable than the milestone versions. This readme is only syncronized with the last milestone version.


# Cheat Sheet

Click on the chapter names below to expand the cheat sheet for that chapter.
Line comments start with `#`.

<details>
  <summary>Math</summary>

    1. a + b ; # Add two numbers

    2. a * b ; # Multiply two numbers

    3. a - b ; # Subtract two numbers

    4. a / b ; # Divide a by b

    5. a ^ b; # Raise a to the power of b

    6. sin(x); cos(x); tan(x); # Trig functions

    7. sinh(x); cosh(x); tanh(x); # Hyperbolic trig functions

    8. sec(x); csc(x); cot(x); # Inverse trig funtions

    9. sech(x); csch(x); coth(x); # Inverse hyperbolic trig functions

    10. abs(x); # Return the absolute value of x

    11. exp(x); # Euler's number, raised to the power of x

    12. ln(x); # Natural log of x

    13. log(b,x); # Log, base b of x

    14. sqrt(x); # Square root of x

    15. random(); # Generate a random number from 0 to 1

    16. seed(number); # Seed the random generator based on an integer

    17. round(); # Nearest integer

    18. diff(:sin(x),:x); # Return the derivative of sin(x) with respect to x

    19. simp(:expr); # Attempt to simplify the given expression

</details>

<details>
  <summary>Equality</summary>

    1. a == b # returns true if a is the same value as b, else, returns false

    2. a > b # returns true if a is more than b, else, returns false

    3. a < b # returns true if a is less than b, else ,returns false

    4. a >= b # returns true if a is more than or equal to b, else ,returns false

    5. a <= b # returns true if a is less than or equal to b, else ,returns false

    6. true is true # the is keyword returns true if both objects are the same instance

</details>

<details>
  <summary>Flow Control</summary>

    1. return x ; # return this value from a function

    2. twice = (x) => 2 * x; # make a function that doubles numbers

    3. quad = (x,a,b,c) => a*x^2+b*x+c; # a quadratic function in x

    4. { command1 ; command2 ; ... }; # A block of commands acts as 1 command

    5. if(condition) command; # execute command if true, else return false

    6. if(condition) command1 else command2; # if condition evaluates to true, executes command1, else executes command2

    7. while(condition) statement # continually repeat statement until condition is false

    8. doWhile ( condition ) statement # Repeating statement until condition is false (checking condition after running statement)

    9. map( function, expression ) # return a new array where each element is the result of applying function to the correlating element of the given expression

    10. not( boolean ) # if boolean is false, returns true, otherwise, returns false

    11. EXPR or EXPR # returns true of either expression is true, else false

    12. EXPR and EXPR # returns true of both expressions are true, else false

    13. exit(n); # quit SMS and return this integer to the OS as the command return value

    14. :sin(x); # capture any expression with the unary meta operator. Use parens to capture more. 

</details>
<details>
  <summary>Contexts</summary>

    1. let var = value ;  # creates a new variable in the current context with the given value.

    2. rm var; # removes the variable from the current context.

    3. var = value; # searches for var in this context, then up the parent path, and if found, sets to value, else a new variable is created in the current context.

    4. let context = { var1 -> value1; var2 -> value2 }; # builds a context with 2 variables and saves it under the variable 'context'

    5. context.var1; # Retreive the value of a specific variable from the context

    6. context.var1 = value; # Set the value of a specific variable from the context

    7. parent(context); # Return the parent scope of the provided context

    8. cxLet(cx,:x,value); # A way to create a new variable in a context or just set it to a new value

    9. cxSet(cx,:x,value); # A way to set cx.x=value for a context cx or return false

    10. cxGet(cx,:x); # Get a value associated with the given key, or return false

    11. cxGetFar(cx,:x,value); # Get the value, searching to higher scopes in the search

    12. cxSize(cx); # Return the number of entries in this context

    13. cxValues(cx); # Return an array with the values of the context

    14. cxKeys(cx); # Return an array with the keys of the context

    15. cxDot(cx,:symbol); # Return this variable from this context

    16. cxContaining(cx,:key); # Returns the context which contains :key, by looking at cx and its ancetry

    17. cxRm(cx, :var); # Remove this entry from the context

    18. cxImport(cx1, cx2); # import the key/value pairs from cx1 into cx2. Overwrites existing key values
</details>

<details>
  <summary>Arrays</summary>

    1. [ expr1, expr2 ] # Create an array by evaluating expressions

    2. :[ expr1 , expr2 ] # Create an array of unevaluated expressions

    3. array[ i ] # Return the i'th  element of the array, where i=0 is the first element

    4. array[ i ] = value # Set the ith value of the array. Returns true upon success only

    5. size( array ) # Return the number of elements in the array

    6. size( expr ) # Return the number of arguments in the expression

</details>

<details>
  <summary>Strings</summary>

    1. let s = "example\nstring"; # s is now a string with a newline escape code (\n)

    2. strFind(s,to_find); # Return the first location of to_find

    3. strSize(s); # Return the length of string s

    4. strEscape(s); # Convert any escape codes into their correlating character

    5. str+(s1,s2); # Return a string that is the concatenation of s1 with s2 in order

    6. strPart(s1,start,len) # Return a part of the string, starting at index start, and with length len

    7. toStr(object0); # Return the string representation of object0

    8. input(); # Allow the user to enter a string of text, which becomes the return value

    9. put(s1); # Print the string s1

    10. putln(s1); # Print the string s1 and go to the next line

    11. strSplit(haystack,needle); # Return an array of strings which are the parts of haystack, split up by instances of needle

</details>

<details>
  <summary>Files</summary>

    1. fileRead("test.txt"); #reads test.txt , paths are relative to the working directory

    2. fileWrite(fname, content); # takes a string for the file name to write to, and a string for the content to write

    3. fileParse(fname); # Parses the file into a single object

    4. cd("..") ; # change working directory

    5. pwd(); # returns the working directory

    6. ls() ; # show files and directories at the current working directory

</details>

<details>
  <summary>Date and Time</summary>

    1. date() # returns the date and time in the form of an array of 9 numbers, listed with their array index:
    # date()[0]: seconds (0-60)
    # date()[1]: minutes (0-59)
    # date()[2]: hours (0-23)
    # date()[3]: Day of month (1-31)
    # date()[4]: months since January (0-11)
    # date()[5]: Years since 1900
    # date()[6]: Days since Sunday (0-6)
    # date()[7]: Days since January 1 (0-365)
    # date()[8]: Dayslights Savings flag (positive if daylight savings is in effect, 0 if not, negative if this is unknown)

    2. time() # returns an array with 2 values: the number of seconds since January 1, 1970, then the number of microseconds since the last whole second.

    3. dateStr() # returns the date in a 24 character string, like: "Thu Apr  6 01:20:24 2023"

    4. sleep(n) # pause process execution for n milliseconds.

</details>

<details>
  <summary>Processes</summary>

    1. osFork(); # returns a process number. If the number is 0, we are in the child process. if the number is more than 0, we are in the same process as before, and the id is the id of the child process. If the number is -1, then forking failed. Check sms_src/fork.sms for an example.

    2. osWait(); # If a child process is running, this waits until the process terminates, then returns an array with 2 elements: a process id, and the return code. If there is no child process to wait for, this function returns [-1,1]. Use this function from the parent of a child process made by osFork(). Check sms_src/fork.sms for an example.

    3. osExec("path/to/binary"); # Execute another file, and wait until the file returns. The return value of osExec is the return value of the process.

    4. exit(n) ; # exit SMS with return code specified by n

</details>



# How to Download and Run/Install a Release
- At the [ Releases page ](https:#github.com/reginaldford/sms/releases)  , under 'Assets', you can find binary executable files for Linux, OpenBSD, and FreeBSD You may rename the file to 'sms' and copy to anywhere you need. On most Linux/Unix systems, copying to somewhere like /usr/local/bin directory is a fine way to install the program. The program is small and portable, so you can have copies where necessary.


# How to Compile for Latest Features
Required Packages:

- Flex

- Bison

- GCC or CLANG compiler

- GNU Make ( or compatible build program )

The makefile uses clang by default.
Edit the first lines of the makefile to use your preferred compiler and make program if necessary.

Once you have the necessary packages installed (GCC/Clang, Flex, and Bison),
Change directory to the top level of the project (where LICENSE.txt is located) and run:

`make`

Upon success, you will have a portable executable called 'sms' in the bin directory.

On Linux/Unix systems, You may be able to build and install in one step:

`sudo make install`

This command builds the executable and copies it to /usr/local/bin/sms


# Language Features, Specs and Design:
- [x] Mathematical evaluation. Commands like `a=4*sin(PI/8);` will evaluate to a double precision decimal number.
- [x] Copying garbage collector. Allows for the entire program memory to compact itself into a small contiguous block after each command.
- [x] Lexical scope.
- [x] SMS command flags allow for running scripts, changing memory heap size, immediate expression evaluation at the command line and more.
- [x] Custom memory heap size can be set from 10 kilobytes to 4 terrabytes.
- [x] User contexts (objects), which allow for the user to create a new context with values of all types including more contexts.
- [x] Safe and fast internal string manipulations, with no character counting and no intermediate buffers for expression printing.
- [x] Turing completeness.
- [x] Recursive function calls.
- [x] Local variables are implemented as array indices, making them relatively fast.
- [x] Large expressions can be parsed and processed with this program. If a collection item is too large to parse, it's because: 1) The memory available at the time of execution is too low. 2) You are parsing more than 4.29 billion elements/characters into a single array or string, or: 3) You are using `-m 4000000` to provide the maximum heap size (4 terrabytes), and have used all of the memory while parsing. Also, contexts have a max capacity of 64^32 entries and contexts do not need to be moved in memory to grow to any size.
- [x] `diff` command for automatic differentiation. Use `diff(:(any_expression),:any_symbol)` and SMS will return the derivative of `any_expression` with respect to `any_symbol`.
- [x] `simp` command for simplifying expressions. Use `simp(:(any_expression))` and SMS will return a simplified version of `any_expression` or it will return the input.
- [x] Example scripts are located in `sms_src` directory.
- [x] Process forking support. Use the osFork() command to build multiprocessing SMS programs. Also, use osExec to run other programs.
- [x] Variable mapping uses mapped tries. Variable lookup times are reasonably fast and remain constant for any size context.
- [x] Syntax errors are useful and line-specific, whether from terminal or interpreting a file.


# Plans:
- [ ] Support for booting from a serialized memory heap.
- [ ] Support for matices. Inversion, solving, eigenvalues, etc.
- [ ] 'Last moment' garbage collection.
- [ ] Tail call optimization.
- [ ] Modern terminal support
- [ ] Dynamic heap size, with min, max, and max_emptiness specs.
- [ ] An integral command for taking integrals.
- [ ] Arbitrary precision support.
- [ ] General documentation.
- [ ] Some Unicode support.
- [ ] Simple Webserver and networking tools.
- [ ] A community!

# Great Ways to Contribute:
- If you would like to contribute to this project and to the open source community, you can talk with me or even just clone the repo and come to me with a pull request, for which I can merge your work into a branch etc. It will be lovely when the contributor count is higher than 1. Any of the unchecked things in the plans above is an area welcome for contributions.

