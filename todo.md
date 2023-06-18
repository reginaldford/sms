! fix vert.sms on multiple usages in 1 session
# single var anonymous func sugar
- x => x^2;

# expression chapter
- get op symbol from an xp
- set op symbol for an xp

# active terminal support
- This code is being developed in the feature/smartput branch
- if non-canonical input is supported, we use smartput()
- non-canonical form allows keyboard shortcuts
  - ctrl+left, ctrl+right to jump 1 token left,right
  - up and down to scroll through previous commands
  - home, end to go to beginning/end
  - alt+left, alt+right to go to beginning/end
  - tab completion

# eager eval for cx
- contexts need eager eval

# unary func notation
- let a = x => x^2;
- a x ;

# data structures
- `let myType = struct({
    age = 5;       # no decimal means int. 64 bits for now
    height=171.5;  # 64 bit float
    name = "none"; # varlen types use ptrs
  });`
- yields `struct(myType)`
- let x = <myType>{ ... nondefaults and additions ... }

# vert.sms
- vvert.sms for volume conversions
- dvert.sms for data conversions
- rvert for rate conversions

# functions and callstack
- Ability to return early from a function
- Stack variables
- Higher order functions use this syntax:
  `
  let diff = ( :x ) => eval(x); #x is only evaluated once
  `
# optional eager eval
- let add=(:x,:y)=> eval(x)+eval(y);
- allows for literal passing without meta operator


# array ops
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

# switch cases
- evaluate the nth element of an array

# argument access
- _args in rooftop

# memoized functions
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
- large byte array: byteArr@<len>
- large numArray: numArr@<len>
- toStr does full mode, no prettyprinting
- toStrPretty takes an optional cx of options?
- color mode eventually.

# internal power series engine
- to cover missing trig functions
- offer functions to carry out the ps method
- build the aux var funcs, reusing internal parts

# last moment gc
- requires explicit stack. use sm_stack.c
- implement tail call opt
- option to move objects outside of heap. malloc(obj)
  - bigSpace = malloc(byteArr@34);
  - bigSpace = !malloc(newByteArr(34,0x33)); # build in outterspace in parser time
  - for large objects, they can be in malloc space
  - gc will not move these objects.
  - if you process the object, it will come back to heap. use malloc(:newStr(...,...)); which will change the allocator for the evaluation of the expression.
    if you use !malloc(:expr) , you can build a large object outside of the heap, and the gc will clear malloc'd objects accordingly

# dynamic heap size
- allow min, max, max_emptiness, growth factor options
- no gc means static heap size bc heap size changes after gc to
  satisfy max_emptiness by growing heap by growth factor.

# typed heaps
- object locations are s.t. type info is in leading bits of object's location as a ptr
- so we have n objects, which need log(2,n) bits to address.
- Heap sizes are set at 2^n bytes from a known ptr,
- size of heap starts at 2^8 = 256 bytes.
- size of the heap doubles if necessary (with realloc)
- speed critical apps can set the size of each heap
- each heap would have their own gc
- each heap can have destination gc, which is set to itself for now
- later, we can have 3-gen typed heaps. That's (num*types)*3 heaps.

# types, typed arrays
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

# matrices
- convert to array
- m is 2d:
k = m[1, 2, 3;
      2, 3, 4;
      3, 4, 5];
- elements of a matrix are accessed with underscore:
k_2,3
- another choice is a tuple of num arrays.
- mOne(x,y);
- k m*3 ;   #matrix multiplication
- k me/ 3 ;  #inversion, solving.
- mInvert(matix) #
- k me*3 ;  #elementwise operations for
  + - * / ^ exp ln log trigs abs etc
  mabs(matrix) for unary funcs
  m+(a,b,c) and a m+ b for binary funcs. no need for me+
  m* me* is multiplication and elementwise, respectively
- mmap( fn(x,y), m) # elementwise map
- mRow(m,r);
- mCol(m,r);
- mapCols( fn(c), m) # column mapping
- mapRows( fn(r), m) # row mapping
- mdet( m ) #determinant
- mToStr is for speed, compared to toStr
- m== is for speed, compared to ==
- mToArr is tuple form output
- mToNumArr    is 1d
- mToNumArrArr is 2d
- mToByteArr
- mToByteArrArr

# multimedia libs
- tgaMaker library
- cimgui lib bindings
- sdl2 bindings

# bison/flex independance plan
- write an sms parser in sms
- parse the parser
- save the image
- will be part of compilation/bootstrap plan

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

