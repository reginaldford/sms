# STATUS QUO:
Compiling this C project will produce a terminal program that interprets commands like `a=4;` and `delete a;` to set and destroy variables, respectively.

To exit the program, use `ctrl + c` or enter `exit;` The syntax is expected to rapidly change for a while. 
The variable names must start with a letter and variables can store equations, strings, numbers, and arrays of those forementioned things.
After setting a variable, the garbage collector is executed, where all relevant objects are copied to a new contiguous space. This is a stop and copy garbage collector. 

You can try the following examples:

`a = sinh(a/b) + c ;`

`b = sqrt(d/e) - 2 * a ;` 

`c = + (1,2,sin(b)) ;`

`d = [ 1,a ,"hello" ,*(a,b,c)];`

`delete c; `

`exit; `


# PLANS:
- Currently, there is no evaluation other than setting and unsetting variables. The only commands are SETVAR ( setting a variable like 'a' to a value like 5 (`a=5;`)) and DELETE (`delete a;`) . The right hand side is not evaluated, and will stores the abstract syntax tree without modification. Instead, the right hand side should be evaluated. A syntax needs to allow for encapsulating the math when necessary. This will be the introduction of an evalutation engine.
- Currently, the garbage collector only executes after a command. The plan is to have the garbage collector initiate when a new memory allocation would pass a threshold. Live objects must be tracked on a stack, which becomes a new member of the roots... No pun intended. The garbage collector would then run less often.
- Currently, the garbage collector is not generational. The plan is to have an n-generation stop and copy garbage collector.
- Better support for scripting and pipes by using flags for input and output preferences.
- A diff command for taking derivatives.
- A integral command for taking integrals.
- Support for floats and integers as opposed to just doubles.
- Quad precision support.
- User contexts, which allow for the user to create a new context with lexical scoping.
- Memory import support (starting with a populated heap from the file system for faster booting).
- Clean the string manipulations to avoid sprintf and strlen
- General Guide.
- Guide for adding your own command by adding C code.
- Turing completeness.
- Arbitrary precision library written in sms.
- Graphics support.

# HOW TO COMPILE

Requirements:
Flex
Bison
GCC or CLANG compiler
GNU MAKE compatible build program

By default, the makefile is set to use clang. Change the comments to switch the file to use gcc if necessary.


To build, run 
`make`
While in the directory of the makefile.

# HOW TO INSTALL:
If you are ok with installing the binary to /usr/bin/sms , then enter:
`sudo make install`
Otherwise, just copy the file to your prefered path.
