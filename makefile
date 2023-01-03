#Must have gcc/clang, make, flex , bison

#Uncomment this to use gcc
# CC=gcc

#OR Uncomment this to use llvm+clang
CC=clang

#OR Uncomment this if you are using gcc on OpenBSD:
# CC=egcc

#Where would you put your binary executable?
INSTALL_DIR=/usr/bin

#Compiling for speed:
CFLAGS= -lm -Ofast

#Compiling for debugging info
#CFLAGS=-lm -g

SOURCES=src/lex.yy.c src/y.tab.c src/sm_object.c src/sm_symbol.c src/sm_memory_heap.c src/sm_string.c src/sm_array.c src/sm_expression.c src/sm_double.c src/sm_context.c src/sm_commands.c src/sm_global.c src/sm_gc.c src/sm_terminal.c src/sm_key_value.c

HEADERS=src/sms.h src/sm_object.h src/sm_symbol.h src/sm_memory_heap.h src/sm_string.h src/sm_array.h src/sm_expression.h src/sm_double.h src/sm_context.h src/sm_global.h src/sm_gc.h src/sm_terminal.h src/sm_key_value.h

all: sms

sms: src/y.tab.c src/lex.yy.c $(SOURCES) $(HEADERS)
	$(CC) -o sms $(SOURCES) $(CFLAGS)

#y.tab.h is also produced by bison.
src/y.tab.c: src/sms.y
	bison -dy -Wcounterexamples src/sms.y
	mv y.tab.c src
	mv y.tab.h src

src/lex.yy.c: src/y.tab.h src/y.tab.c src/sms.l
	flex --yylineno src/sms.l
	mv lex.yy.c src

install: all
	cp -fv sms /usr/bin/sms
	chmod +x /usr/bin/sms

clean:
	rm -v src/lex.yy.c src/y.tab.c src/y.tab.h sms
