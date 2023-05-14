# functions and callstack
- Ability to return early from a function
- Stack variables

# singleton symbol set
- global cx , scratchpad terminology. _scratchpad is a global
- Make the _all_ symbols global to keep uniqueness for symbols
- The right side will be a refcount, so we can delete symbols with 0 references at gc.
- After gc, update globals for true, false etc.
- then, we can optimize symbol equivilance tests to ptr equivilance tests.
- note that == should be true if the toStr(obj1)==toStr(obj2) but 'is' keyword will check for ptr equality.
- so loops will use things like while(x is true) or while( n < size is not(true));

# reflexivity
- get op symbol from an expr
- set op symbol for an expr

# context return values
- a return command will change the whole surrounding context into the return value.

# break statement
- to end a while

# argument access
- _args in rooftop


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
k = m[ 1, 2, 3;
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

# heap loading/saving
- Saving a heap to a file
- loading the file as a live heap
- loading a heap as an inspectable object
- Heap conversion, editing, and exporting tool

# dynamic heap size
- allow min, max, max_emptiness, growth factor options
- no gc means static heap size bc heap size changes after gc to
  satisfy max_emptiness by growing heap by growth factor.

# parse time functions
- !fname(a1,a2..); #that's all there is to it.

# dimensions lib
- let ft = : 12`in ; # ratio setting example
- let in = : / ( ft ,  12 ) ; # ratio setting example
- let a = 3`ft `+ 6`ft       # = 9`ft #example. so 3e2%e is
- a `to :in ; # => 108`in
- ` md backticks are not being used correctly here, for sure

# extensibility
- custom objects
  - custom toStr
  - custom type name
  - memory block unwrapping
- c abi / ffi
- cgi wrapper

# multimedia libs
- tgaMaker library
- cimgui lib bindings
- sdl2 bindings

# bison/flex independance plan
- Build an internal parser generator
- describe the same grammar defined in sms.y
- save the resulting memory image as a starting point

#compilation and cross compilation
- The engine will call a single function for every case, and SMS will have an array of pointers to each engine case function.
then , primitive calls would specify the function to call by its array index, and they would be pre-computed becuase primitives do not change.
- Some functions will need to be created to manage the memory heap, such as changing a primtive number, etc.
- Once memory cross-compiling is done, we can leave bison and make sms work on any supported arch from any other supported arch.
- This work will be encapsulated in the cross-compilation library. At this point
