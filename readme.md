# Introduction:
SMS (Symbolic Math System) is a terminal based calculator. SMS has built-in functions for arithmetic operations, flow control, trigonometry, algebra, and calculus. SMS aims to become a simple programming language that focuses on mathematics and speed. SMS is still in early stages of development. SMS can be downloaded and installed from the Releases page on GitHub. 

# Longer Introduction:

In addition to providing a command line, SMS can interpret files.
Run `sms -h` for command line options.

Unlike Javascript, Math functions in SMS are global keywords.

For example, taking a square root in javascript may look like `Math.sqrt(x)`. In SMS, this same function is permanently available as  `sqrt(x)`. Another major difference between SMS and javascript is that SMS provides the meta operator (`:`) which allows developers to capture expressions for metaprogramming.

SMS has the following built-in functions (math operations are in double precision):

Arithmetical operations: `+, -, *, /, ^, ln, exp, abs,  sqrt` 

Trigonometry: `sin, cos, tan, sinh, cosh, tanh, sec, csc, cot, sech, csch, coth`

Algebra: `simp` for simplifying expressions. Uses a multipass algorithm, which is constantly being improved.

Calculus: `diff` for taking derivatives.

Flow control: `if, map, while, eval, ==, >, >=, <, <=, exit`

Strings: `strcat, strlen, parse, to_string`

Files: `file_read , file_parse, file_write`

A help command with explanations and examples for each SMS command will be created soon.
For now, we have the list above and the tutorial below.

# How to Download and Run/Install a Release
- At the [ Releases page ](https://github.com/reginaldford/sms/releases)  , under 'Assets', you can find binary executable files for Linux, OpenBSD, and FreeBSD You may rename the file to 'sms' and copy to anywhere you need. On most Linux/Unix systems, copying to somwhere like /usr/bin directory is a fine way to install the program. The program is small and portable, so you can have copies where necessary.


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

# Plans:
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


# Tutorial:
You can learn the basics of SMS by trying the following commands in order.

Once you have the SMS prompt running, you can enter the following commands. Note: In SMS, whitespace characters (outside of strings) are ignored. All commands end with a semicolon (`;`).

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

User objects (contexts) which are key-value stores:

`car = { weight = 2.2; speed = 5; license_plate = "smthg_fnny"; } ; `

Contexts can be nested:

`nested = { example = { x = [ 1, "two" ]; }; k = 5.123; } ;`

Access fields of objects:

`nested.example.x;`

Obtain the parent of an object:

`parent(self);`

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

Exit SMS with ctrl+c, or exit with a specific code:

`exit(0);`

Check out `sms_src/example.sms` for example functions and ideas.
You can also run `sms -i example.sms` to load SMS with useful functions and constants.

Array element access, field access,`size`,`parse`,`eval`, `parent`, `diff`,`simp` are not in the 0.14 release and will be in the 0.15 release.
SMS still lacks a package management system, proper syntax error reporting, and many other features of a mature scripting language. As these features are added, this README will be updated.
