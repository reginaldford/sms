#Must have gcc/clang, make, flex , bison

#Where would you put your binary executable?
INSTALL_DIR=/usr/bin

#Uncomment this to use gcc
# CC=gcc

#OR Uncomment this to use llvm+clang
CC=clang

#OR Uncomment this if you are using gcc on OpenBSD:
# CC=egcc

#OR targeting Windows from linux
# CC=x86_64-w64-mingw32-gcc


#Compiling for speed:
CFLAGS= -lm -Ofast

#Compiling for debugging info
# CFLAGS=-lm -g

#ncurses experiments:
#CFLAGS= -lm -lncurses -Ofast


SOURCES=src/lex.yy.c src/y.tab.c src/sm_object.c src/sm_symbol.c src/sm_memory_heap.c src/sm_string.c src/sm_expression.c src/sm_double.c src/sm_context.c src/sm_commands.c src/sm_global.c src/sm_gc.c src/sm_terminal.c src/sm_key_value.c src/sm_pointer.c src/sm_engine.c src/sm_meta.c src/sm_signal.c

HEADERS=               src/sms.h src/sm_object.h src/sm_symbol.h src/sm_memory_heap.h src/sm_string.h src/sm_expression.h src/sm_double.h src/sm_context.h src/sm_commands.h src/sm_global.h src/sm_gc.h src/sm_terminal.h src/sm_key_value.h src/sm_pointer.h src/sm_engine.h src/sm_meta.h src/sm_signal.h

all: sms

sms: src/y.tab.c src/lex.yy.c $(SOURCES) $(HEADERS)
	$(CC) -o sms $(SOURCES) $(CFLAGS)

#For counterexamples during bison conflicts:
#Some versions of bison do not have this
#bison -dy -Wcounterexamples -o src/y.tab.c src/sms.y
#y.tab.h is also produced by bison.
src/y.tab.c: src/sms.y
	bison -dy -o src/y.tab.c src/sms.y

src/lex.yy.c: src/y.tab.h src/y.tab.c src/sms.l
	flex --yylineno -o src/lex.yy.c src/sms.l

install: all
	cp -fv sms /usr/bin/sms
	chmod +x /usr/bin/sms

clean:
	rm -v src/lex.yy.c src/y.tab.c src/y.tab.h sms
