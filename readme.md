# Intro:

SMS (Symbolic Math System) is a calculator that can perform double-precision arithmetic, trigonometry, and symbolic differentiation. It aims to become a simple untyped programming language that focuses on mathematics and speed. SMS has built-in functions for arithmetic operations, flow control, trigonometry, algebra, and calculus. SMS has features like anonymous functions, contexts, and custom memory heap size. It is Turing complete and will eventually have standard libraries. SMS can be downloaded and installed from the Releases page on GitHub. 

In addition to providing a command line, SMS can interpret files.
Run `sms -h` for command line options.

Contexts are user defined objects with key-value pairs. Creating and running functions in SMS appears much like javascript's arrow-function syntax. Where javascript would have `Math.sqrt`, SMS has `sqrt`.

SMS has the following built-in functions (math operations are in double precision):

Arithmetical operations: `+, -, *, /, ^, ln, exp, abs,  sqrt` 

Flow control: `if, ==, >, <, map`

Trigonometry: `sin, cos, tan, sinh, cosh, tanh, sec, csc, cot, sech, csch, coth`

Algebra: `simp` for simplifying expressions. Uses a multipass algorithm, which is constantly being improved.

Calculus: `diff` for taking derivatives. Example below.

# HOW TO DOWNLOAD & RUN / Install
- At the [ Releases page ](https://github.com/reginaldford/sms/releases)  , under 'Assets', you can find binary executable files for Linux, OpenBSD, and FreeBSD You may rename the file to 'sms' and copy to anywhere you need. On most Linux/Unix systems, copying to somwhere like /usr/bin directory is a fine way to install the program. The program is small and portable, so you can have copies where necessary.

In SMS, whitespace characters (outside of strings) are ignored. All commands end with a semicolon (`;`) .


# Features

Check out `sms_src/example.sms` for example functions and ideas.
You can also run `sms -i example.sms` to load SMS with useful functions and constants.

In addition to the mathematical ideas covered in the example file, SMS has ambitions to be a general purpose programming language. Hence, SMS has premature general purpose programming features that will be embellished further in the near future:

Anonymous functions with javascript-like syntax:

`wave = (x) => sin(x) + 0.5 * cos(x * 2);`

Call the function with familiar syntax:

`wave(4);`

See the variables you have set with the self command:

`self;`

Take the derivative of any math expression:

`diff(:sin(x),:x);`

Simplify an expression:

`simp(:(a*x^(5-4)),:x);`

User objects (contexts):

`car = { weight = 2.2; speed = 5; license_plate = "smthg_fnny"; } ; `

Contexts can be nested:

`nested = { example = { x = [ 1, "two" ]; }; k = 5.123; } ;`

Access fields of objects:

`nested.example.x`

Obtain the parent of an object:

`self.parent`

If statements:

`if(nested.example.x[0] == 1, "It's a 1","It's not a 1");`

Map statements:

`map((x)=>x^2+1,[1,2,3,4]);`

Access array elements:

`x = [ 1, "two" , "3" ];`

`x[2];`

Get the size of an array:

`size(x);`

Variables can be removed from the current context:

`rm x;`

Exit the program with:

`exit;`

Array element access, field access, parent command, diff, and simp are all new and will be in the 0.15 release.
SMS still lacks a package management system, proper type checking, and many features of a more mature language. As these features are added, this README will be updated.
Type checking is essentially not being done for function calls, so we have undefined behaviour for errors like `"notANumber" + 5`;


# FEATURES:
- [x] Mathematical evaluation. Commands like `a=4*sin(3.14/8);` will evaluate to a double precision decimal number.
- [x] Copying garbage collector. Allows for the entire program memory to compact itself into a small contiguous block after each command.
- [x] Lexical scope.
- [x] Command line flags to run scripts or initialize into a loaded REPL.
- [x] Useful example.sms file with common conversion functions and constants.
- [x] Custom memory heap size can be set from 100 kilobytes to 1 terrabyte.
- [x] User contexts, which allow for the user to create a new context with values of all types including more contexts.
- [x] Safe and fast internal string manipulations, with no character counting and no intermediate buffers for expression printing.
- [x] Turing completeness. Store variables, run inequality tests, loops, if-statements. Note that, though the control flow is there, many library functions are missing. For now, you can use `if` for conditionals and `map` (examples above);
- [x] Recursive function calls.
- [x] Local variables are implemented as array indices, making them fast.
- [x] Huge expressions can be parsed with this program. If a collection item is too large to parse, it's because: 1) The memory available at the time of execution is too low. 2) You are parsing more than 4.29 billion elements/characters or: 3) You are legitimately reaching the max heap size of 1 terrabyte of memory for SMS (successfully using -m 1000000).
- [x] A `diff` command for taking derivatives. Use `diff(:(any_expression),:any_symbol)` and SMS will return the derivative of `any_expression` with respect to `any_symbol`. (This feature is in the repo, not in the latest release);
- [x] A `simp` command for simplifying expressions. Use `simp(:(any_expression))` and SMS will return a simplified version of `any_expression` or it will return the input. (This feature is in the repo, not in the latest release);

# PLANS:
- [ ] Standard libraries: file, string, array, math, matrix, net, etc.
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
