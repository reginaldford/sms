![GitHub](https://img.shields.io/github/license/reginaldford/sms)
![GitHub top language](https://img.shields.io/github/languages/top/reginaldford/sms) 
![GitHub language count](https://img.shields.io/github/languages/count/reginaldford/sms)
![GitHub all releases](https://img.shields.io/github/downloads/reginaldford/sms/total)
![GitHub contributors](https://img.shields.io/github/contributors/reginaldford/sms?color=green)
![GitHub commit activity](https://img.shields.io/github/commit-activity/m/reginaldford/sms)
![img](https://img.shields.io/badge/maturity-alpha-orange)
![GitHub repo size](https://img.shields.io/github/repo-size/reginaldford/sms)
![GitHub Workflow Status](https://img.shields.io/github/actions/workflow/status/reginaldford/sms/c-cpp.yml)

# Introduction:
SMS (Symbolic Math System) is a terminal based calculator. SMS has built-in functions for arithmetic operations, programmatic flow control, trigonometry, algebra, and calculus. SMS aims to become a simple programming language that focuses on mathematics and speed. SMS is still in early stages of development. SMS can be downloaded and installed from the Releases page on GitHub. The latest source code may have undocumented changes and this document is synchronized with the project at the point of the latest release: ![GitHub latest release (latest by date)](https://img.shields.io/github/v/tag/reginaldford/sms)

# Longer Introduction:

In addition to providing a command line, SMS can interpret files.
Run `sms -h` for command line options.

Unlike Javascript, Math functions in SMS are global keywords.

For example, taking a square root in javascript may look like `Math.sqrt(x)`. In SMS, this same function is permanently available as  `sqrt(x)`.

Another major difference between SMS and javascript is that SMS provides the meta operator (`:`) which allows developers to capture expressions for metaprogramming.

For example, taking a derivative of `sin(x)` with respect to `x` may be done with `diff(:sin(x),:x);` where the colon prevents SMS from evaluating the input and ultimately preserving the symbolic expression.

The following is 'hello world' in SMS:

`print("Hello world!");`

SMS versions have 2 significant figures (like 0.150) during a milestone point. At these points, only unit tests are developed and documentation (like this readme) is updated, and a release is created.

SMS versions with 3 significant figures (like 0.155) are in development, so they may lack documentation for new features, and might have unannounced changes to existing features.

Line comments start with `//` which causes the remainder of the line to be ignored.


# Cheat Sheet

Click on the chapter names below to expand the cheat sheet for that chapter.

<details>
  <summary>Math</summary>

    1. a + b ; // add two numbers

    2. +( a, b, c) ; // 2 or more numbers can be added with prefix notation

    3. a * b ; // mulitply two numbers

    4. *( a, b, ..); // multiply 2 or more numbers

    5. a - b ; // subtract two numbers

    6. -(a,b...) ; //substract remaining numbers from the first

    7. a / b ; //divide a by b

    8. /(a,b,...) //divide a by b, then divide by remaining numbers

    9. a ^ b; // raise a to the power of b

    10. sin(x); cos(x); tan(x); // trig functions

    11. sinh(x); cosh(x); tanh(x); // hyperbolic trig functions

    12. sec(x); csc(x); cot(x); // inverse trig funtions

    13. sech(x); csch(x); coth(x); // inverse hyperbolic trig functions

    14. abs(x); //return the absolute value of x

    15. exp(x); //Euler's number, raised to the power of x

    16. ln(x); //natural log of x

    17. sqrt(x); //square root of x

    18. random(); //generate a random number from 0 to 1

    19. diff(:sin(x),:x); //return the derivative of sin(x) with respect to x

    20. simp(:expr); // attempt to simplify the given expression

</details>

<details>
  <summary>Equality</summary>

    1. a == b // returns true if a is the same value as b, else, returns false

    2. a > b // returns true if a is more than b, else, returns false

    3. a < b // returns true if a is less than b, else ,returns false

    4. a >= b // returns true if a is more than or equal to b, else ,returns false

    5. a <= b // returns true if a is less than or equal to b, else ,returns false

</details>

<details>
  <summary>Flow Control</summary>

    1. twice = (x) => 2 * x; // make a function that doubles numbers

    2. quad = (x,a,b,c) => a*x^2+b*x+c; //a quadratic function in x

    3. ( command1 ; command2 ; ... ) // this unites multiple commands into 1 command. Does not make a new scope

    4. if(condition, command); // execute command if true, else return false

    5. if(condition, command1, command2); // if condition evaluates to true, executes command1, else executes command2

    6. while(condition , statement) // continually repeat statement until condition is false

    7. map( function, expression ) // return a new array where each element is the result of applying function to the correlating element of the given expression

    8. not( boolean ) // if boolean is false, returns true, otherwise, returns false

    9. exit(n); // quit SMS and return this integer to the OS as the command return value

    10. :sin(x); //capture any expression with the unary meta operator. Use parens to capture more.

</details>
<details>
  <summary>Contexts</summary>

    1. let var = value ;  // creates a new variable in the current context with the given value.

    2. rm var; // removes the variable from the current context.

    3. var = value; // searches for var in this context, then up the parent path, and if found, sets to value, else a new variable is created in the current context.

    4. context = { var1 = value1; var2 = value2 }; // builds a context with 2 variables and saves it under the variable 'context'

    5. context.var1; // retreive the value of a specific variable from the context

    6. parent(context); // return the parent scope of the provided context
</details>

<details>
  <summary>Arrays</summary>

    1. [ expr1, expr2 ] // create an array by evaluating expressions

    2. :[ expr1 , expr2 ] // create an array of unevaluated expressions

    3. array[ i ] // return the i'th  element of the array, where i=0 is the first element

    4. size( array ) // return the number of elements in the array

    5. size( expr ) // returns the number of arguments in the expression

</details>

<details>
  <summary>Strings</summary>

    1. let s = "example\nstring"; // s is now a string with a newline escape code (\n)

    2. str_find(s,to_find);       // returns the first location of to_find

    3. str_len(s);  // returns the length of string s

    4. str_escape(s); // converts any escape codes into their correlating character

    5. str_add(s1,s2); // returns a string that is the concatenation of s1 with s2 in order

    6. str_part(s1,start,len) // returns a part of the string, starting at index start, and with length len

    7. to_string(object0); // return the string representation of object0

    8. input(); // allow the user to enter a string of text, which becomes the return value

    9. print(s1); //print the string s1

    10. println(s1); //prnt the string s1 and go to the next line


</details>

<details>
  <summary>Files</summary>

    1. file_read("test.txt"); //reads test.txt , paths are relative to the working directory

    2. file_write(fname, content); // takes a string for the file name to write to, and a string for the content to write

    3. file_parse(fname); // Parses the file into a single object

</details>

<details>
  <summary>Date and Time</summary>

    1. date() // returns the date and time in the form of an array of 9 numbers, listed with their array index:
    //0: seconds (0-60)
    //1: minutes (0-59)
    //2: hours (0-23)
    //3: Day of month (1-31)
    //4: months since January (0-11)
    //5: Years since 1900
    //6: Days since Sunday (0-6)
    //7: Days since January 1 (0-365)
    //8: Dayslights Savings flag (positive if daylight savings is in effect, 0 if not, negative if this is unknown)

    2. time() // returns an array with 2 values: the number of seconds since January 1, 1970, then the number of microseconds since the last whole second.

    3. date_str() // returns the date in a 24 character string, like: "Thu Apr  6 01:20:24 2023"

    4. sleep(n) // pause execution for n milliseconds.

</details>



# How to Download and Run/Install a Release
- At the [ Releases page ](https://github.com/reginaldford/sms/releases)  , under 'Assets', you can find binary executable files for Linux, OpenBSD, and FreeBSD You may rename the file to 'sms' and copy to anywhere you need. On most Linux/Unix systems, copying to somwhere like /usr/local/bin directory is a fine way to install the program. The program is small and portable, so you can have copies where necessary.


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

This command builds the executable and copies it to /usr/bin/sms


# Language Features, Specs and Design:
- [x] Mathematical evaluation. Commands like `a=4*sin(3.14/8);` will evaluate to a double precision decimal number.
- [x] Copying garbage collector. Allows for the entire program memory to compact itself into a small contiguous block after each command.
- [x] Lexical scope.
- [x] Command line flags to run scripts or initialize into a loaded REPL.
- [x] Useful example.sms file with common conversion functions and constants.
- [x] Custom memory heap size can be set from 100 kilobytes to 1 terrabyte.
- [x] User contexts, which allow for the user to create a new context with values of all types including more contexts.
- [x] Safe and fast internal string manipulations, with no character counting and no intermediate buffers for expression printing.
- [x] Turing completeness.
- [x] Recursive function calls.
- [x] Local variables are implemented as array indices, making them fast.
- [x] 'Huge' expressions can be parsed and processed with this program. If a collection item is too large to parse, it's because: 1) The memory available at the time of execution is too low. 2) You are parsing more than 4.29 billion elements/characters into a single array or string, or: 3) You are using `-m 1000000` to provide the maximum heap size (1 terrabyte), and have used all of the memory while parsing.
- [x] `diff` command for automatic differentiation. Use `diff(:(any_expression),:any_symbol)` and SMS will return the derivative of `any_expression` with respect to `any_symbol`. (This feature is in the repo, not in the latest release);
- [x] `simp` command for simplifying expressions. Use `simp(:(any_expression))` and SMS will return a simplified version of `any_expression` or it will return the input. (This feature is in the repo, not in the latest release);
- [x] Example scripts are located in `sms_src` directory.

# Plans:
- [ ] Support for booting from a serialized memory heap.
- [ ] 'Last moment' garbage collection.
- [ ] Tail call optimization.
- [ ] Package system.
- [ ] Precise syntax error reporting.
- [ ] Modern terminal support
- [ ] Try/Catch error syntax.
- [ ] Dynamic heap size, with min, max, and max_emptiness specs.
- [ ] An integral command for taking integrals.
- [ ] Arbitrary precision support.
- [ ] General documentation.
- [ ] Thread forking support.
- [ ] Some Unicode support.
- [ ] Simple Webserver and networking tools.
- [ ] A community!

