# This makefile is designed for GNU Make


# Seems to work with zig cc, clang, gcc, egcc (OpenBSD), tcc
INSTALL_DIR     := /usr/local/bin
CC              := clang
CC_DEBUG        := clang
CC_PROF         := clang
CC_UNIFIED      := zig cc
CFLAGS_UNIFIED  := -Oz
#CFLAGS_UNIFIED := -Oz --target=x86_64-linux -static -I$(MUSL_PREFIX)/include 
CFLAGS          := -O3 -ffast-math
CFLAGS_DEBUG    := -g
CFLAGS_PROF     := -fprofile-instr-generate -fcoverage-mapping
LDFLAGS         := -lm -flto -lffi
#LDFLAGS        := -lm -flto -static
BUILD_DIR       := build
SRC_BISON_FLEX  := src/bison_flex
SRC_MAIN        := src/main
SRC_PARSER      := src/main/parser
SRC_OBJECT      := src/main/object
SRC_ENGINE      := src/main/engine
SRC_MEM         := src/main/memory
SRC_TERMINAL    := src/main/terminal
SRC_TEST        := src/test
SRC_KERN_TEST   := src/kernel_test
SRCS_MAIN := $(filter-out $(SRC_MAIN)/sm_main.c $(SRC_MAIN)/linenoise/example.c, $(shell find $(SRC_MAIN) -name '*.c'))
SRCS_TEST       := $(shell find $(SRC_TEST) -name '*.c')
SRCS_KT         := $(shell find $(SRC_KERN_TEST) -name '*.c')
OBJS_PARSER     := $(BUILD_DIR)/$(SRC_BISON_FLEX)/y.tab.c.o $(BUILD_DIR)/$(SRC_BISON_FLEX)/lex.yy.c.o
OBJS_PARSER_DBG := $(BUILD_DIR)/$(SRC_BISON_FLEX)/y.tab.c.dbg.o $(BUILD_DIR)/$(SRC_BISON_FLEX)/lex.yy.c.dbg.o
OBJS_PARSER_PROF:= $(BUILD_DIR)/$(SRC_BISON_FLEX)/y.tab.c.prof.o $(BUILD_DIR)/$(SRC_BISON_FLEX)/lex.yy.c.prof.o
OBJS_BASE       := $(SRCS_MAIN:%=$(BUILD_DIR)/%.o)       # base files
OBJS_BASE_DBG   := $(SRCS_MAIN:%=$(BUILD_DIR)/%.dbg.o)   # dbg version of base files
OBJS_BASE_PROF  := $(SRCS_MAIN:%=$(BUILD_DIR)/%.prof.o)  # prof version of base files
OBJS_TEST       := $(SRCS_TEST:%=$(BUILD_DIR)/%.dbg.o)   # sms tests
OBJS_KT         := $(SRCS_KT:%=$(BUILD_DIR)/%.dbg.o)     # sms kernel tests

BIN_NAME        := sms
BIN_NAME_UNIFIED:= sms_unified
BIN_NAME_DBG    := sms_dbg
BIN_NAME_TEST   := sms_test
BIN_NAME_KT     := sms_kernel_test
BIN_NAME_PROF   := sms_prof
THREADS         := 8 #match the number of threads on your machine
DOCS_CONFIG     := docs/docs.conf

# sms executable
main:
	$(MAKE) $(SRC_MAIN)/linenoise/linenoise.c
	$(MAKE) $(SRC_BISON_FLEX)/y.tab.c
	$(MAKE) $(SRC_BISON_FLEX)/lex.yy.c
	$(MAKE) -j$(THREADS) bin/$(BIN_NAME)

# linenoise
$(MAKE) $(SRC_MAIN)/linenoise/linenoise.c:
	git submodule update --recursive --init

# Additional libraries in C


# Rule to build the shared library
build/clib/fib.so:
	$(CC) -shared  -I ./src/main/ clib/fib.c -o $@ $^

# Rule to compile each source file to an object file
$(OBJDIR)/%.so: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

clibs: build/clib/fib.so




# Not files
.PHONY: all

# Makes all targets except prof
all:
	$(MAKE) main
	$(MAKE) -j$(THREADS) bin/$(BIN_NAME_TEST)
	$(MAKE) -j$(THREADS) bin/$(BIN_NAME_KT)
	$(MAKE) -j$(THREADS) bin/$(BIN_NAME_DBG)
	$(MAKE) -j$(THREADS) build/clib/fib.so

# sms executable
bin/$(BIN_NAME):  $(OBJS_PARSER) $(OBJS_BASE) $(BUILD_DIR)/$(SRC_MAIN)/sm_main.c.o
	$(CC) $(CFLAGS) $(OBJS_BASE) $(OBJS_PARSER) $(BUILD_DIR)/$(SRC_MAIN)/sm_main.c.o -o $@ $(LDFLAGS)

# sms_dbg executable
bin/$(BIN_NAME_DBG): $(OBJS_PARSER_DBG) $(OBJS_BASE_DBG) $(BUILD_DIR)/$(SRC_MAIN)/sm_main.c.dbg.o
	$(CC_DEBUG) $(CFLAGS_DEBUG) $(OBJS_BASE_DBG) $(OBJS_PARSER_DBG) $(BUILD_DIR)/$(SRC_MAIN)/sm_main.c.dbg.o -o $@ $(LDFLAGS)

# sms_test executable
bin/$(BIN_NAME_TEST): $(OBJS_PARSER_DBG) $(OBJS_BASE_DBG) $(OBJS_TEST)
	$(CC_DEBUG) $(CFLAGS_DEBUG) $(OBJS_BASE_DBG) $(OBJS_PARSER_DBG) $(OBJS_TEST) -o $@ $(LDFLAGS) 

# sms_kernel_test executable
bin/$(BIN_NAME_KT): $(OBJS_PARSER) $(OBJS_BASE_DBG) $(OBJS_KT)
	$(CC_DEBUG) $(CFLAGS_DEBUG) $(OBJS_BASE_DBG) $(OBJS_PARSER) $(OBJS_KT) -o $@ $(LDFLAGS)

# Bison generates the parser
$(SRC_BISON_FLEX)/y.tab.c: $(SRC_MAIN)/parser/sms.y
	bison -d $(SRC_MAIN)/parser/sms.y -o $(SRC_BISON_FLEX)/y.tab.c

# Flex generates the lexer
$(SRC_BISON_FLEX)/lex.yy.c: $(SRC_BISON_FLEX)/y.tab.c $(SRC_MAIN)/parser/sms.l
	flex --yylineno -o $(SRC_BISON_FLEX)/lex.yy.c $(SRC_MAIN)/parser/sms.l

# Compile .o files from .c files
$(BUILD_DIR)/%.c.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compile .dbg.o files from .c files
$(BUILD_DIR)/%.c.dbg.o: %.c
	$(CC_DEBUG) $(CFLAGS_DEBUG) -c $< -o $@

# Compile .prof.o files from .c files
$(BUILD_DIR)/%.c.prof.o: %.c
	$(CC_PROF) $(CFLAGS_PROF) -c $< -o $@

# Artificial BIN_NAMEs (not file destinations)
.PHONY: all clean install docs

# Reset file state
clean:
	rm -vrf  $(SRC_BISON_FLEX)/lex.yy.c\
		$(SRC_BISON_FLEX)/y.tab.*\
		$(BUILD_DIR)/$(SRC_BISON_FLEX)/*.o\
		$(BUILD_DIR)/$(SRC_MAIN)/*.o\
		$(BUILD_DIR)/$(SRC_KERN_TEST)/*.o\
		$(BUILD_DIR)/$(SRC_TEST)/*.o\
		$(BUILD_DIR)/$(SRC_PARSER)/*.o\
		$(BUILD_DIR)/$(SRC_OBJECT)/*.o\
		$(BUILD_DIR)/$(SRC_ENGINE)/*.o\
		$(BUILD_DIR)/$(SRC_MEM)/*.o\
		$(BUILD_DIR)/$(SRC_TERMINAL)/*.o\
		$(BUILD_DIR)/$(SRC_MAIN)/linenoise/*.o\
		$(BUILD_DIR)/$(SRC_MAIN)/linenoise/*.o\
		$(BUILD_DIR)/clib/*.so\
		bin/sms*\
		docs/html

# Install the binary to a unix-like system
install: main
	cp -fv bin/$(BIN_NAME) $(INSTALL_DIR)/$(BIN_NAME)
	chmod +x $(INSTALL_DIR)/$(BIN_NAME)

# For developers
dev:
	$(MAKE) $(SRC_BISON_FLEX)/y.tab.c
	$(MAKE) $(SRC_BISON_FLEX)/lex.yy.c
	$(MAKE) -j$(THREADS) bin/$(BIN_NAME) bin/$(BIN_NAME_DBG) bin/$(BIN_NAME_TEST) bin/$(BIN_NAME_KT)

# For profiling
prof:
	$(MAKE) $(SRC_BISON_FLEX)/y.tab.c
	$(MAKE) $(SRC_BISON_FLEX)/lex.yy.c
	$(MAKE) -j$(THREADS) bin/$(BIN_NAME_PROF)

bin/$(BIN_NAME_PROF): $(OBJS_PARSER_PROF) $(OBJS_BASE_PROF) $(BUILD_DIR)/$(SRC_MAIN)/sm_main.c.prof.o
	$(CC_PROF) $(CFLAGS_PROF) $(OBJS_PARSER_PROF) $(OBJS_BASE_PROF) $(BUILD_DIR)/$(SRC_MAIN)/sm_main.c.prof.o -o $@ $(LDFLAGS)

# Run the unit tests
check: dev	
	./bin/$(BIN_NAME_KT) &&\
	./bin/$(BIN_NAME_TEST) test_zone/outline.sms &&\
	echo Kernel Tests and SMS Tests Passed.

# Generate html for docs with doxygen
docs:
	doxygen $(DOCS_CONFIG)

# sms unified binary will be faster
unified:
	$(MAKE) $(SRC_BISON_FLEX)/y.tab.c
	$(MAKE) $(SRC_BISON_FLEX)/lex.yy.c
	$(CC_UNIFIED) $(CFLAGS_UNIFIED) $(SRC_BISON_FLEX)/y.tab.c $(SRC_BISON_FLEX)/lex.yy.c $(SRCS_MAIN) $(SRC_MAIN)/sm_main.c -o bin/$(BIN_NAME_UNIFIED) $(LDFLAGS)

# install the unified version
install_unified: unified
	cp -fv bin/$(BIN_NAME_UNIFIED) $(INSTALL_DIR)/$(BIN_NAME)
	chmod +x $(INSTALL_DIR)/$(BIN_NAME)
