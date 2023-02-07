# Intro:

SMS Stands for Symbolic Math System. The application is a minimal algebraic system and a functional programming language.
The SMS language is lexically scoped, weakly typed, and functional. SMS currently has no compiler.

The C program compiles to a terminal application which interprets commands like `a = 4 * cos(0.5);` and `rm a;` to calculate, set, and destroy variables.

The program currently parses, evaluates, and stores mathematical expressions, strings, and data structures like arrays and contexts. Contexts are user defined objects with key-value pairs. Functions can also be called with lexical scoping for variables and functions.

To exit the program, use `ctrl + c` or enter `exit;` The syntax is expected to rapidly change for a while. The variable names must start with underscores or a letter and variables can store mathematical expressions, strings, numbers, contexts and arrays of the forementioned things.

Supported functions so far (in double precision):

`+, -, *, /, ^, ln, exp, abs, sin, cos, tan, sinh, cosh, tanh, sec, csc, cot, sqrt, if, ==, >, <` 

In SMS, whitespace characters (outside of a string) are ignored. All commands end with a semicolon (;) .
You can try the following examples:

Direct calculations can be performed with any mathematical expression:

`2 + sqrt(7);`

Calculation and storage of double precision numbers:

`a = 2 + sqrt(7)`

`b = sqrt(a) + 1.27E-3;`

`c = (a + b)/2;`

To see which variables are currently set, enter:

`self ;`

Prefix notation may also be used for +,-,*,/.

`c = + ( 1, sin( b ) ) ;`

The meta operator (:) stores expressions :

`expr = : +(a,b,c);`

Call the function later with this syntax:

`expr {}`

Override specific variables:

`expression {b=6}`

Define a function that calculates the nth fibonacci number:

`fib=:if( n < 2 , 1 , fib{ n=n-1 } + fib { n=n-2 } );`

`fib{n=20}`

On AMD64 architecture, this implementation of fib will crash for n > 21 because SMS uses a 5 MB heap size.

Storage of mathematical expressions in an array (a meta array):

`quad = : [ (-b+sqrt(b^2-4*a*c)/(2*a) , (-b+sqrt(b^2-4*a*c)/(2*a) ];`

Call the quadratic formula function and recieve two values.

`quad {a=-1,b=2,c=3};`

Arrays can hold objects of different types:

`d = :[ a, sinh(a/b), "this", 5 , { x = "x"; }];`

User objects (contexts):

`car = { color = red; speed = 5; license_plate = "smthg_fnny"; } ; `

Contexts can be nested:

`nested = { example={ x = [ 1, 2 ]; }; k = 5; } ;`

Variables can be removed from the current context:

`rm c;`

Exit the program with:

`exit; `

# PLANS:

- [x] Mathematical evaluation is implemented. Commands like a=4*sin(3.14/8) will evaluate to a decimal number.
- [ ] Currently, the garbage collector only executes after a command. The plan is to have the garbage collector initiate when a new memory allocation would pass a threshold. Live objects must be tracked on a stack, which becomes a new member of the roots for inflating to the new heap at garbage collection. With this upgrade, the garbage collector would then run less often.
- [ ] Currently, syntax errors are entirely vague and repeat for every token after an error up until the next semicolon. Errors will be handled more gracefully, with useful responses for particular cases.
- [ ] Better support for scripting and pipes by using flags for input and output preferences.
- [ ] A diff command for taking derivatives.
- [ ] A integral command for taking integrals.
- [ ] Support for floats and integers as opposed to just doubles.
- [ ] Quad precision support.
- [x] User contexts, which allow for the user to create a new context with values of all types including more contexts.
- [ ] Memory serialization support (starting with a populated heap from the file system for faster booting).
- [x] Clean the string manipulations to avoid sprintf and strlen.
- [ ] General Guide.
- [ ] Guide for adding your own command by adding C code.
- [x] Turing completeness.
- [ ] Arbitrary precision library written in sms.
- [ ] Unicode support.
- [ ] Graphics support.

# HOW TO DOWNLOAD & RUN / Install
- At the [ Releases page ](https://github.com/reginaldford/sms/releases)  , under 'Assets', you can find binary executable files for Linux, OpenBSD, FreeBSD, and Windows. You may rename the file to 'sms' and copy to anywhere you need. On most Linux/Unix systems, copying to somwhere like /usr/bin directory is a fine way to install the program. The program is small and portable, so you can have copies where necessary.

# HOW TO COMPILE

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

This just copies the executable to /usr/bin/sms after compilation.
