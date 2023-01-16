#Requires gcc/clang, make, flex , bison

#Where would you put your binary executable?
INSTALL_DIR=/usr/bin

# Works with clang, gcc, egcc (OpenBSD), x86_64-w64-mingw32-gcc
CC= clang

#Use your preferred compiler for debugging
CC_DEBUG= clang

#Compiling for speed:
CFLAGS= -lm -Ofast -fshort-enums

#Compiling for debugging info
CFLAGS_DEBUG= -lm -g -fshort-enums

#ncurses experiments:
#CFLAGS= -lm -lncurses -Ofast -fshort-enums

SOURCES=src/lex.yy.c src/y.tab.c src/sm_object.c src/sm_symbol.c src/sm_heap.c src/sm_string.c src/sm_expr.c src/sm_double.c src/sm_context.c src/sm_commands.c src/sm_global.c src/sm_gc.c src/sm_terminal.c src/sm_key_value.c src/sm_pointer.c src/sm_engine.c src/sm_meta.c src/sm_signal.c src/sm_space.c

HEADERS=               src/sms.h src/sm_object.h src/sm_symbol.h src/sm_heap.h src/sm_string.h src/sm_expr.h src/sm_double.h src/sm_context.h src/sm_commands.h src/sm_global.h src/sm_gc.h src/sm_terminal.h src/sm_key_value.h src/sm_pointer.h src/sm_engine.h src/sm_meta.h src/sm_signal.h src/sm_space.h

all: sms

sms: src/y.tab.c src/lex.yy.c $(SOURCES) $(HEADERS)
	$(CC) -o sms $(SOURCES) $(CFLAGS)

debug: src/y.tab.c src/lex.yy.c $(SOURCES) $(HEADERS)
	$(CC_DEBUG) -o sms $(SOURCES) $(CFLAGS_DEBUG)


#y.tab.h is also produced by bison.
src/y.tab.c: src/sms.y
	bison -dy -o src/y.tab.c src/sms.y
#bison -dy -Wcounterexamples -o src/y.tab.c src/sms.y
#^ For counterexamples during bison conflicts:
#Some versions of bison do not have this

src/lex.yy.c: src/y.tab.h src/y.tab.c src/sms.l
	flex --yylineno -o src/lex.yy.c src/sms.l

install: all
	cp -fv sms /usr/bin/sms
	chmod +x /usr/bin/sms

clean:
	rm -v src/lex.yy.c src/y.tab.c src/y.tab.h sms
