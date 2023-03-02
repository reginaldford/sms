# Intro:

SMS Stands for Symbolic Math System.

Currently, SMS can serve as calculator for double-precision arithmetic, trigonometry and differentiation. For a while, SMS will be a practical calculator. The long term goal of SMS is to provide a simple general purpose programming language that focuses on mathematics and speed.

In addition to providing a command line, SMS can interpret files.
Run `sms -h` for command line options.

Contexts are user defined objects with key-value pairs. Creating and running functions in SMS appears much like javascript's arrow-function syntax. Where javascript would have `Math.sqrt`, SMS has `sqrt`.

SMS has the following built-in functions so far (in double precision):

Arithmetical operations: `+, -, *, /, ^, ln, exp, abs,  sqrt` 

Flow control: `if, ==, >, <`

Trigonometry: `sin, cos, tan, sinh, cosh, tanh, sec, csc, cot,sech,csch,coth`

Calculus: `diff`

# HOW TO DOWNLOAD & RUN / Install
- At the [ Releases page ](https://github.com/reginaldford/sms/releases)  , under 'Assets', you can find binary executable files for Linux, OpenBSD, and FreeBSD You may rename the file to 'sms' and copy to anywhere you need. On most Linux/Unix systems, copying to somwhere like /usr/bin directory is a fine way to install the program. The program is small and portable, so you can have copies where necessary.

In SMS, whitespace characters (outside of strings) are ignored. All commands end with a semicolon (`;`) .

Check out `sms_src/example.sms` for example functions and ideas.
You can also run `sms -i example.sms` to have a calculator loaded with some useful functions and constants.

In addition to the mathematical ideas covered in the example file, SMS has ambitions to be a general purpose programming language. Hence, SMS has premature general purpose programming features that will be embellished further in the near future:

Anonymous functions with javascript-like syntax:

`wave = (x) => sin(x) + 0.5 * cos(x * 2);`

Call the function with familiar syntax:

`wave(4);`

User objects (contexts):

`car = { weight = 2.2; speed = 5; license_plate = "smthg_fnny"; } ; `

Contexts can be nested:

`nested = { example = { x = [ 1, "two" ]; }; k = 5.123; } ;`

Variables can be removed from the current context:

`rm c;`

Exit the program with:

`exit; `

The above features are premature because, though we can create nested objects and nested arrays, there is no way to refer to things inside an object from the outside and there is no way to access the elements of an array. As these features are added, this README will be updated. On the other hand, the anonymous functions can be created and called to make complex procedures already. Just note that type checking is essentially not being done for function calls, so we have undefined behaviour for errors like `"notANumber" + 5`;


# FEATURES:
- [x] Mathematical evaluation. Commands like a=4*sin(3.14/8) will evaluate to a double precision decimal number.
- [x] Copying garbage collector.
- [x] Lexical scope.
- [x] Command line flags to run scripts or initialize into a loaded REPL.
- [x] Useful example.sms file with common conversion functions and constants.
- [x] Custom memory heap size can be set from 100 kilobytes to 1 terrabyte.
- [x] User contexts, which allow for the user to create a new context with values of all types including more contexts.
- [x] Fast string manipulation techniques, that avoid unsafe character iteration algorithms.
- [x] Only 2 uses of malloc in the interpreter: for the 2 heaps of the copying GC algorithm.
- [x] Turing completeness. Store variables, run inequality tests, loops, if-statements. Note that, though the control flow is there, many library functions are missing.
- [x] Recursive function calls.
- [x] Local variables store an array index to their location in the stack frame. This is much faster than binary search.
- [x] The maximum capacity for strings, arrays, and contexts is remarkably high. Generally, if a collection item is too large to parse, it's because: 1) The memory available at the time of execution is too low. 2) You are parsing more than 4.29 billion elements/characters or: 3) You are legitimately reaching the max heap size of 1 terrabyte of memory for SMS (successfully using -m 1000000).
- [x] A `diff` command for taking derivatives. Use `diff(:(any_expression),:any_symbol)` and SMS will return the derivative of `any_expression` with respect to `any_symbol`. (This feature is in the repo, not in the latest release);

# PLANS:
- [ ] Configurable algebraic simplification.
- [ ] Standard libraries: file, string, array, math, matrix, net, etc.
- [ ] Support for booting from a serialized memory heap.
- [ ] 'Last moment' garbage collection.
- [ ] Tail call optimization.
- [ ] Package system.
- [ ] Precise syntax error reporting.
- [ ] Modern terminal support
- [ ] Try/Catch error syntax.
- [ ] Variable heap size, with min, max, and max_emptiness specs.
- [ ] A integral command for taking integrals.
- [ ] Arbitrary precision support.
- [ ] General documentation.
- [ ] A community!
- [ ] Thread forking support.
- [ ] Some Unicode support.
- [ ] Simple Webserver and networking tools.


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
