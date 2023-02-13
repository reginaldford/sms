# Requires gcc/clang, make, flex and bison
# On OpenBSD, use gmake command instead of make

# Your preferred directory for the binary installation
INSTALL_DIR=/usr/bin

# Works with clang, gcc, egcc (OpenBSD), x86_64-w64-mingw32-gcc
CC=clang

# Use your preferred compiler for debugging
CC_DEBUG=clang

# Compiling for speed:
CFLAGS=-fshort-enums -O3

# Compiling for debugging info
CFLAGS_DEBUG=-fshort-enums -g

# ncurses experiments:
#CFLAGS= -lm -lncurses -Ofast -fshort-enums

SOURCES=src/lex.yy.c src/y.tab.c src/sm_main.c src/sm_object.c src/sm_symbol.c src/sm_heap.c src/sm_string.c src/sm_expr.c src/sm_double.c src/sm_context.c src/sm_commands.c src/sm_global.c src/sm_gc.c src/sm_terminal.c src/sm_pointer.c src/sm_engine.c src/sm_meta.c src/sm_signal.c src/sm_space.c src/sm_stack.c src/sm_parse_result.c src/sm_init.c src/sm_fun.c

HEADERS=src/y.tab.h src/sm_main.h src/sms.h src/sm_object.h src/sm_symbol.h src/sm_heap.h src/sm_string.h src/sm_expr.h src/sm_double.h src/sm_context.h src/sm_commands.h src/sm_global.h src/sm_gc.h src/sm_terminal.h src/sm_pointer.h src/sm_engine.h src/sm_meta.h src/sm_signal.h src/sm_space.h src/sm_stack.h src/sm_parse_result.h src/sm_init.h src/sm_fun.h

OBJ_FILES=src/lex.yy.o src/y.tab.o src/sm_main.o src/sm_object.o src/sm_symbol.o src/sm_heap.o src/sm_string.o src/sm_expr.o src/sm_double.o src/sm_context.o src/sm_commands.o src/sm_global.o src/sm_gc.o src/sm_terminal.o src/sm_pointer.o src/sm_engine.o src/sm_meta.o src/sm_signal.o src/sm_space.o src/sm_stack.o src/sm_parse_result.o src/sm_init.o src/sm_fun.o

DEBUG_OBJ_FILES=src/lex.yy.o src/y.tab.o src/sm_object.dbg.o src/sm_symbol.dbg.o src/sm_heap.dbg.o src/sm_string.dbg.o src/sm_expr.dbg.o src/sm_double.dbg.o src/sm_context.dbg.o src/sm_commands.dbg.o src/sm_global.dbg.o src/sm_gc.dbg.o src/sm_terminal.dbg.o src/sm_pointer.dbg.o src/sm_engine.dbg.o src/sm_meta.dbg.o src/sm_signal.dbg.o src/sm_space.dbg.o src/sm_stack.dbg.o src/sm_parse_result.dbg.o src/sm_init.dbg.o src/sm_fun.dbg.o

# Optimized executable

bin/sms: $(OBJ_FILES)
	$(CC) -lm -o bin/sms $(OBJ_FILES)

%.o : %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

%.dbg.o : %.c %.h
	$(CC_DEBUG) $(CFLAGS_DEBUG) -c $< -o $@

	
src/y.tab.c src/y.tab.h: src/sms.y
	bison -dy -o src/y.tab.c src/sms.y

# For counterexamples during bison conflicts:
#bison -dy -Wcounterexamples -o src/y.tab.c src/sms.y

src/lex.yy.c: src/y.tab.h src/y.tab.c src/sms.l
	flex --yylineno -o src/lex.yy.c src/sms.l


# Annotated executable

bin/sms_debug: $(DEBUG_OBJ_FILES) src/sm_main.dbg.o
	$(CC_DEBUG) $(CFLAGS_DEBUG) -lm -o bin/sms_debug $(DEBUG_OBJ_FILES) src/sm_main.dbg.o

# sms_test runs unit tests

TEST_SOURCES=src/test/sm_test.c src/test/sm_test_outline.c
TEST_HEADERS=src/test/sm_test.h src/test/sm_test_outline.h
TEST_OBJ_FILES=src/test/sm_test.dbg.o src/test/sm_test_outline.o

bin/sms_test: $(TEST_OBJ_FILES) $(DEBUG_OBJ_FILES) $(TEST_HEADERS)
	$(CC_DEBUG) $(CFLAGS_DEBUG) -lm -o bin/sms_test $(TEST_OBJ_FILES) $(DEBUG_OBJ_FILES)

# Kernel test program runs tests on internal functions

kernel_test_SOURCES=src/kernel_test/chapter_0.c src/kernel_test/chapter_1.c src/kernel_test/kernel_test.c
kernel_test_HEADERS=src/kernel_test/chapter_0.h src/kernel_test/chapter_1.h
kernel_test_OBJ_FILES=src/kernel_test/kernel_test.dbg.o src/kernel_test/chapter_0.dbg.o src/kernel_test/chapter_1.dbg.o

src/kernel_test/kernel_test.dbg.o: $(kernel_test_SOURCES) $(kernel_test_HEADERS)
	$(CC_DEBUG) $(CFLAGS_DEBUG) -c src/kernel_test/kernel_test.c -o src/kernel_test/kernel_test.dbg.o

src/kernel_test/%.dbg.o: %.c %.h
	$(CC_DEBUG) $(CFLAGS_DEBUG) -c $< -o $@

bin/sms_kernel_test: $(kernel_test_OBJ_FILES) $(DEBUG_OBJ_FILES)
	$(CC_DEBUG) $(CFLAGS_DEBUG) -lm -o bin/sms_kernel_test $(kernel_test_OBJ_FILES) $(DEBUG_OBJ_FILES)

all: bin/sms bin/sms_debug bin/sms_test bin/sms_kernel_test

install: bin/sms
	cp -fv bin/sms $(INSTALL_DIR)/sms
	chmod +x $(INSTALL_DIR)/sms

clean:
	rm -v -f  src/lex.yy.c src/y.tab.c src/y.tab.h src/*.o src/test/*.o src/*.o src/kernel_test/*.o bin/sms bin/sms_debug bin/sms_test
