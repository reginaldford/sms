
# osExec alternative where the output is returned as a string

# bytes and byte ops build in
- sm_mem object
- m
- \x00 etc
- &
- ||
- <<, >>
- 

# Use -v <n> for verbosity level, no more -q

# add semicolon for convenience for parse cmds

# cx
- save and load

# active terminal support
- Will use linenoise eventually

# data structures built in sms
- `let myType = struct({
    age -> 5;        # no decimal means int. 64 bits for now
    height -> 171.5;  # 64 bit float
    name -> "none";  # varlen types use ptrs
  });`
- yields `struct(myType)`
- `let x = <myType>{ ... nondefaults and additions ... }`

# vert.sms upgrades
- vvert.sms for volume conversions
- dvert.sms for data conversions
- rvert for rate conversions

# vectormath lib
- arr+ !! etc for parallel tuple add
- numArr+ etc for parallel vec add
- byteArr+ etc for parallel vec add
- cat is for concatenating
- part is for partition
- repeat is for repeating
- reverse is for repeating
- select takes a boolean map function
  - select( (x)=>x<5, arr); # is generic part function!
    - returns the same kind of array
  - selectAssoc( (x)=>x<5, arr1, arr2, arr3..); # selects the same from arr2, etc.
    - returns arrArr

# break statement
- to end a while
- can just make a block around the loop and return early though

# switch cases
- evaluate the nth element of an array

# memoized functions library
- allow for:
  - `let x = memoize((input) => (
      # your code here;
    ));`

# heap loading/saving
- Saving a heap to a file
- loading the file as a live heap
- loading a heap as an inspectable object
- Heap conversion, editing, and exporting tool

# parse time functions
- !fname(a1,a2..); #that's all there is to it.

# prettyPrinting
- matrices: short mode shows m@x,y, smart mode will truncate if matrices have the same height
- options for max width
- beyond this , system uses [ x , x .... x , x ] #symmetric printouts
- full mode will show whole matrices, but return line if they are more than a certain amount
- long strings will show as str@<len>
- long arrays will show as arr@<len>
- large contexts will show as cx@<len>
:- large byte array: byteArr@<len>
- large numArray: numArr@<len>
- toStr does full mode, no prettyprinting
- toStrPretty takes an optional cx of options?

# power series engine library (mostly in C)
- to cover missing trig functions
- offer functions to carry out the ps method
- build the aux var funcs, reusing internal parts

# last moment gc
- requires explicit stack. use sm_stack.c
- Now we have stack tracing inside sms

# mark and sweep gc
- because we eventually want generational, and this is a step.
- involves making a high order function for tracing 
- since every object is on a mod 4 byte in memory, we have 2 bits to signify special meaning.
- one bit is for fixnums, the other is for marking
- sweep is a state machine that builds spaces for allocation later.
- spaces are a set of 64 pointer stacks, to 4, 8, 12,... 256 bytes.
- If the sweep machine passes over 256 bytes of garbage, it will make a space for 256, and start a new one.

# external memory library
- option to evaluate code, instantiate/move objects outside of heap.
  - bigSpace = malloc(byteArr@34);
  - for large objects, they can be in malloc space
  - gc will not move these objects.
  - Allow for evaluating code with a specified external heap: eval_in_heap(:(code),heap1)

# dynamic heap size
- allow min, max, max_emptiness, growth factor options
- no gc means static heap size bc heap size changes after gc to
  satisfy max_emptiness by growing heap by growth factor.

# types, typed arrays (will be implemented as a library)
- str[ ...] makes a single string , concatenating the strings
with an array of integers for their start locations.
- similar tools for other types of arrays
- byte type #
- byte[ fa , aa , 03 ...] array prints as hex like 0xFA, has func to change hex to dec.
- char type #
- char array is string. "string"
- uchar type # unicode strings!
- uchar arrays
- int type with prefix i, like i45
- type casting # only state desired type, like toStr=(str), so deprecate toStr
- num array is 1d

# matrices lib (std lib that offers c bindings for fast matrix ops in sms)
- convert to array
- m is 2d:
`k = m[1, 2, 3;
      2, 3, 4;
      3, 4, 5];`
- elements of a matrix are accessed with underscore:
`k_2,3`
- another choice is a tuple of num arrays.
- mOne(x,y);
- `k m*3 ;   #matrix multiplication`
- `k me/ 3 ;  #inversion, solving.`
- `mInvert(matix)` #
- `k me*3 ;`  #elementwise operations for
  + - * / ^ exp ln log trigs abs etc
  mabs(matrix) for unary funcs
  m+(a,b,c) and a m+ b for binary funcs. no need for me+
  m* me* is multiplication and elementwise, respectively
- `mmap( fn(x,y), m)` # elementwise map
- `mRow(m,r);`
- `mCol(m,r);`
- `mapCols( fn(c), m)` # column mapping
- `mapRows( fn(r), m)` # row mapping
- `mdet( m )` #determinant
- `mToStr` is for speed, compared to toStr
- `m==` is for speed, compared to ==
- `mToArr` is tuple form output
- `mToNumArr`    is 1d
- `mToNumArrArr` is 2d
- `mToByteArr`
- `mToByteArrArr`

# multimedia libs
- tgaMaker library
- cimgui lib bindings
- sdl2 bindings

# bison/flex independance plan
- Write a parser combinator lib in C for sms syntax
- Instead of hardcoding the standard library, 
- require `import std;`
- parser will look up words for std functions, just like any function. through `sm_cx_get` , etcS


#compilation and cross compilation
- aiming for llvm IR
- emit llvm ir into memory, and compile
- compile( (x)=>x^2 ); # we get compiled( ... )

# dimensions lib
- let ft = : 12`in ;` # ratio setting example
- let in = : / ( ft ,  12 ) ; # ratio setting example
- let a = 3`ft `+ 6`ft       # = 9`ft #example. so 3e2%e is
- a `to :in ; # => 108`in

# extensibility
- custom objects
  - custom toStr
  - custom type name
  - memory block unwrapping
- c abi / ffi
- python bindings
- cgi wrapper

