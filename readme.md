# STATUS QUO:

SMS is a terminal program that interprets commands like `a = 4 * cos(0.5);` and `delete a;` to set and destroy variables, respectively.

The program currently parses, evaluates, and stores mathematical expressions, strings, and data structures like arrays and contexts. Contexts are user defined objects with key-value pairs.

To exit the program, use `ctrl + c` or enter `exit;` The syntax is expected to rapidly change for a while. The variable names must start with underscores or a letter and variables can store mathematical expressions, strings, numbers, contexts and arrays of the forementioned things.

After a command, the garbage collector is executed. The garbage collector uses the stop and copy algorithm. 

The whitespace characters in the provided examples are optional, and are for readability. All commands end with a semicolon (;) .
You can try the following examples:

Direct computations. The resulting value will not be stored for later use:

`2 + sqrt(7);`

Calculation and storage of double precision numbers:

`a = 4E-2;`

`b = sqrt(a);`

`c = a + b;`

To see which variables are currently set, enter:

`self ;`

Hyperbolic trigonometric expressions:

`a = sinh(a/b) + c ;`


The meta operator (:) stores expressions :

`expression = : +(a,b,c);`

Store structured data with no evaluation:

`x= : {name="power graph"; value=cos(b);};`

Removing the meta operator (:) above will lead to a different result.

Storage of mathematical expressions in an array:

`formula = : [ a ^ 2 + b ^ 2 , ln(a/b) ];`

Prefix sums:

`c = + ( 1, sin( b ) ) ;`

Set many variables to the same object:

`a=b=c=d={same="object";};`

Arrays can hold various objects:

`d = :[ a, sinh(a/b), "this", 5 , { x = "x"; }];`

Without the meta operator above, each element of the array will be evaluated.

User objects (contexts):

`car = { color = red; speed = 5; license_plate = "smthg_fnny"; } ; `

Contexts can be nested:

`nested = { example={ x = [ 1, 2 ]; }; k = 5; } ;`

Variables can be removed from the current context:

`delete c;`

Define a function:

`parabolic=:*(a,x^2);`

`a=4;`

`x=2;`

After setting `a` and `x`, the following will call the `parabolic` function. 

`parabolic{};`

Since there are no variables provided in the context following 'parabolic', the dynmic scoping algorithm will go up to the global context for the values associated with the keys `a` and `x`.

The values can be overridden individually:

`parabolic { a = 2 ; };`

`parabolic { a = 2 ; x = 1 ; };`


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
- [ ] Turing completeness.
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

- GNU Make compatible build program

The makefile uses clang by default.
Edit the first lines of the makefile to use your preferred compiler and make program if necessary.

Once you have the necessary packages installed,
Change directory to the top level of the project (where LICENSE.txt is located) and run:

`make`

Upon success, you will have a portable executable called 'sms'.

On Linux/Unix systems, You may be able to build and install in one step:

`sudo make install`

This just copies the executable to /usr/bin/sms after compilation.
