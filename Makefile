# This makefile is designed for GNU Make

# Seems to work with zig cc, clang, gcc, egcc (OpenBSD)
CC              := clang
CC_DEBUG        := clang
CC_PROF         := clang
CFLAGS          := -fshort-enums -O3
CFLAGS_DEBUG    := -fshort-enums -g
CFLAGS_PROF     := -fshort-enums -fprofile-instr-generate -fcoverage-mapping
BUILD_DIR       := build
SRC_KERN_TEST   := src/kernel_test
SRC_TEST        := src/test
SRC_BISON_FLEX  := src/bison_flex
SRC_MAIN        := src/main
SRC_PARSER      := src/main/parser
SRC_OBJECT      := src/main/object
SRC_ENGINE      := src/main/engine
SRC_MEM         := src/main/memory
SRCS1           := $(filter-out $(SRC_MAIN)/sm_main.c, $(shell find $(SRC_MAIN) -name '*.c'))
SRCS2           := $(shell find $(SRC_TEST) -name '*.c')
SRCS3           := $(shell find $(SRC_KERN_TEST) -name '*.c')
OBJS0           := $(BUILD_DIR)/$(SRC_BISON_FLEX)/y.tab.c.o $(BUILD_DIR)/$(SRC_BISON_FLEX)/lex.yy.c.o
OBJS5           := $(BUILD_DIR)/$(SRC_BISON_FLEX)/y.tab.c.dbg.o $(BUILD_DIR)/$(SRC_BISON_FLEX)/lex.yy.c.dbg.o
OBJS7           := $(BUILD_DIR)/$(SRC_BISON_FLEX)/y.tab.c.prof.o $(BUILD_DIR)/$(SRC_BISON_FLEX)/lex.yy.c.prof.o
OBJS1           := $(SRCS1:%=$(BUILD_DIR)/%.o)     # base files
OBJS2           := $(SRCS2:%=$(BUILD_DIR)/%.dbg.o) # sms tests
OBJS3           := $(SRCS3:%=$(BUILD_DIR)/%.dbg.o) # sms kernel tests
OBJS4           := $(SRCS1:%=$(BUILD_DIR)/%.dbg.o) # dbg version of base files
OBJS6           := $(SRCS1:%=$(BUILD_DIR)/%.prof.o)# prof version of base files
INCLUDE         := include
INSTALL_DIR     := /usr/local/bin
BIN_NAME        := sms
BIN_NAME_DBG    := sms_dbg
BIN_NAME_TEST   := sms_test
BIN_NAME_KT     := sms_kernel_test
BIN_NAME_PROF   := sms_prof
THREADS         := 8 #match the number of threads on your machine

# sms executable
main:
	$(MAKE) -j$(THREADS) bin/$(BIN_NAME)

bin/$(BIN_NAME): $(OBJS0) $(OBJS1) $(BUILD_DIR)/$(SRC_MAIN)/sm_main.c.o
	$(CC) -lm $(OBJS1) $(OBJS0) $(BUILD_DIR)/$(SRC_MAIN)/sm_main.c.o -o $@

# Not files
.PHONY: all

# Makes all targets except prof
all:
	$(MAKE) -j$(THREADS) bin/$(BIN_NAME)
	$(MAKE) -j$(THREADS) bin/$(BIN_NAME_TEST)
	$(MAKE) -j$(THREADS) bin/$(BIN_NAME_KT)
	$(MAKE) -j$(THREADS) bin/$(BIN_NAME_DBG)

# sms_dbg executable
bin/$(BIN_NAME_DBG): $(OBJS5) $(OBJS4) $(BUILD_DIR)/$(SRC_MAIN)/sm_main.c.dbg.o
	$(CC_DBG) -lm $(OBJS4) $(OBJS5) $(BUILD_DIR)/$(SRC_MAIN)/sm_main.c.dbg.o -o $@

# sms_test executable
bin/$(BIN_NAME_TEST): $(OBJS5) $(OBJS4) $(OBJS2)
	$(CC_DEBUG) $(CFLAGS_DEBUG) -lm $(OBJS4) $(OBJS5) $(OBJS2) -o $@

# sms_kernel_test executable
bin/$(BIN_NAME_KT): $(OBJS0) $(OBJS4) $(OBJS3)
	$(CC_DEBUG) $(CFLAGS_DEBUG) -lm $(OBJS4) $(OBJS0) $(OBJS3) -o $@

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
.PHONY: all clean install

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
		bin/sms*

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

bin/$(BIN_NAME_PROF): $(OBJS7) $(OBJS6) $(BUILD_DIR)/$(SRC_MAIN)/sm_main.c.prof.o
	$(CC_PROF) $(CFLAGS_PROF) -lm $(OBJS7) $(OBJS6) $(BUILD_DIR)/$(SRC_MAIN)/sm_main.c.prof.o -o $@

# Run the unit tests
check: dev	
	./bin/$(BIN_NAME_KT) &&\
	./bin/$(BIN_NAME_TEST) test_zone/outline.sms &&\
	echo Kernel Tests and SMS Tests Passed.

