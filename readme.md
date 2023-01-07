# STATUS QUO:
SMS is a terminal program that interprets commands like `a=4;` and `delete a;` to set and destroy variables, respectively.
The program currently parses, evaluates, and stores mathematical expressions, strings, and data structures like arrays and contexts.

To exit the program, use `ctrl + c` or enter `exit;` The syntax is expected to rapidly change for a while. 
The variable names must start with underscores or a letter and variables can store mathematical expressions, strings, numbers, contexts and arrays of the forementioned things.
After a command, the garbage collector is executed, where all relevant objects are copied to a new contiguous space. The garbage collector uses the stop and copy algorithm. 

The spaces in provided examples are optional, and are there for readability.
All commands end with a semicolon.
You can try the following examples:

Direct computations. The resulting value will not be stored for later use:
`2+sqrt(7);`

Calculation and storage of double precision numbers:

`a=4;`

`b=sqrt(7);`

`c=a+b;`

Hyperbolic trigonometric expressions:

`a = sinh(a/b) + c ;`

Demonstration of standard order of operations (look at the output):

`b = sqrt(a/b) - 2 * a ^ 2 ;` 

Store an expression with the meta operator (:) :
`expression = : +(a,b,c);`

Store structured data with no evaluation:
`x=5;`
`:{name="power graph"; value=cos(x);};`

Removing the meta operator (:) above will lead to a different result.

Storage of mathematical expressions in an array:

`formula = : [ a ^ 2 + b ^ 2 , cos(a/b) ];`

Prefix sums:

`c = + ( 1, sin( b ) ) ;`

Arrays can hold various objects:

`d = :[ a, sinh(a/b), "this", 5 , { x = "x"; }];`

Without the meta operator above, each element of the array will be evaluated.

User objects (contexts):

`car = { color = red; speed = 5; license_plate = "smthg_fnny"; } ; `

Contexts can be nested:

`nested = { example={ x = [ 1, 2 ]; }; k = 5; } ;`

Variables can be removed from the current context:

`delete c; `

Exit the program with:

`exit; `


# PLANS:

- [x] Mathematical evaluation is implemented. Commands like a=4*sin(3.14/8) will evaluate to a decimal number.
- [ ] Currently, the garbage collector only executes after a command. The plan is to have the garbage collector initiate when a new memory allocation would pass a threshold. Live objects must be tracked on a stack, which becomes a new member of the roots for inflating to the new heap at garbage collection. With this upgrade, the garbage collector would then run less often.
- [ ] Currently, the garbage collector is not generational. The plan is to have an n-generation stop and copy garbage collector. With will make the garbage collector run less often for long-term objects.
- [ ] Currently, syntax errors are entirely vague and repeat for every token after an error up until the next semicolon. Errors will be handled more gracefully, with useful responses for particular cases.
- [ ] Better support for scripting and pipes by using flags for input and output preferences.
- [ ] A diff command for taking derivatives.
- [ ] A integral command for taking integrals.
- [ ] Support for floats and integers as opposed to just doubles.
- [ ] Quad precision support.
- [x] User contexts, which allow for the user to create a new context with values of all types including more contexts.
- [ ] Memory serialization support (starting with a populated heap from the file system for faster booting).
- [ ] Clean the string manipulations to avoid sprintf and strlen.
- [ ] General Guide.
- [ ] Guide for adding your own command by adding C code.
- [ ] Turing completeness.
- [ ] Arbitrary precision library written in sms.
- [ ] Unicode support.
- [ ] Graphics support.


# HOW TO COMPILE

Requirements:

- Flex

- Bison

- GCC or CLANG compiler

- GNU MAKE compatible build program

By default, the makefile is set to use clang.
Edit the first lines of the makefile to set for your compiler and make program if necessary.

Once you have the necessary packages installed,
Change directory to the top level of the project (where readme.md is located) and run:
`make`

# HOW TO INSTALL:
If you are ok with installing the binary to /usr/bin/sms , then enter:
`sudo make install`
Otherwise, just copy the file to your prefered path.
