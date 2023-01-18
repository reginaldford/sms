# Requires gcc/clang, make, flex and bison
# On OpenBSD, use gmake command instead of make

# Where would you put your binary executable?
INSTALL_DIR=/usr/bin

# Works with clang, gcc, egcc (OpenBSD), x86_64-w64-mingw32-gcc
CC=clang

# Use your preferred compiler for debugging
CC_DEBUG=clang

# Compiling for speed:
CFLAGS=-fshort-enums -O3

# Compiling for debugging info
CFLAGS_DEBUG=-lm -fshort-enums -g

# ncurses experiments:
#CFLAGS= -lm -lncurses -Ofast -fshort-enums

SOURCES=src/lex.yy.c src/y.tab.c src/sm_object.c src/sm_symbol.c src/sm_heap.c src/sm_string.c src/sm_expr.c src/sm_double.c src/sm_context.c src/sm_commands.c src/sm_global.c src/sm_gc.c src/sm_terminal.c src/sm_key_value.c src/sm_pointer.c src/sm_engine.c src/sm_meta.c src/sm_signal.c src/sm_space.c

HEADERS=src/sms.h src/y.tab.h    src/sm_object.h src/sm_symbol.h src/sm_heap.h src/sm_string.h src/sm_expr.h src/sm_double.h src/sm_context.h src/sm_commands.h src/sm_global.h src/sm_gc.h src/sm_terminal.h src/sm_key_value.h src/sm_pointer.h src/sm_engine.h src/sm_meta.h src/sm_signal.h src/sm_space.h

OBJ_FILES=src/lex.yy.o src/y.tab.o src/sm_object.o src/sm_symbol.o src/sm_heap.o src/sm_string.o src/sm_expr.o src/sm_double.o src/sm_context.o src/sm_commands.o src/sm_global.o src/sm_gc.o src/sm_terminal.o src/sm_key_value.o src/sm_pointer.o src/sm_engine.o src/sm_meta.o src/sm_signal.o src/sm_space.o

sms: $(OBJ_FILES)
	$(CC) -lm -o sms $(OBJ_FILES)

%.o : %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@
	
sms_debug: src/y.tab.c src/lex.yy.c $(SOURCES) $(HEADERS)
	$(CC_DEBUG) -o sms_debug $(SOURCES) $(CFLAGS_DEBUG)

src/y.tab.c src/y.tab.h: src/sms.y
	bison -dy -o src/y.tab.c src/sms.y

# For counterexamples during bison conflicts:
#bison -dy -Wcounterexamples -o src/y.tab.c src/sms.y

src/lex.yy.c: src/y.tab.h src/y.tab.c src/sms.l
	flex --yylineno -o src/lex.yy.c src/sms.l

all: sms

install: all
	cp -fv sms /usr/bin/sms
	chmod +x /usr/bin/sms

clean:
	rm -v -f  src/lex.yy.c src/y.tab.c src/y.tab.h src/*.o sms sms_debug
