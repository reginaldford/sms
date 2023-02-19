# This makefile is designed for GNU Make

# Tested with clang, gcc, egcc (OpenBSD), x86_64-w64-mingw32-gcc
CC              := clang
CC_DEBUG        := clang
CFLAGS          := -fshort-enums -O3
CFLAGS_DEBUG    := -fshort-enums -g
BUILD_DIR       := build
SRC_DIR0        := src/bison_flex
SRC_DIR1        := src/main
SRC_DIR2        := src/kernel_test
SRC_DIR3        := src/test
SRCS1           := $(filter-out $(SRC_DIR1)/sm_main.c, $(shell find $(SRC_DIR1) -name '*.c'))
SRCS2           := $(shell find $(SRC_DIR3) -name '*.c')
SRCS3           := $(shell find $(SRC_DIR2) -name '*.c')
OBJS0           := $(BUILD_DIR)/$(SRC_DIR0)/y.tab.c.o $(BUILD_DIR)/$(SRC_DIR0)/lex.yy.c.o
OBJS1           := $(SRCS1:%=$(BUILD_DIR)/%.o)		  # base files
OBJS2           := $(SRCS2:%=$(BUILD_DIR)/%.dbg.o)  # kernel tests
OBJS3           := $(SRCS3:%=$(BUILD_DIR)/%.dbg.o)	# tests
OBJS4           := $(SRCS1:%=$(BUILD_DIR)/%.dbg.o)	# dbg version of base files
INCLUDE         := include
INSTALL_DIR     := /usr/bin
TARGET_SMS      := sms
TARGET_TEST     := sms_test
TARGET_KT       := sms_kernel_test

# sms executable
bin/$(TARGET_SMS): $(OBJS0) $(OBJS1) $(BUILD_DIR)/$(SRC_DIR1)/sm_main.c.o 
	$(CC) -lm $(OBJS1) $(OBJS0) $(BUILD_DIR)/$(SRC_DIR1)/sm_main.c.o -o $@

# sms_test executable
bin/$(TARGET_TEST): $(OBJS0) $(OBJS4) $(OBJS2)
	$(CC_DEBUG) $(CFLAGS_DEBUG) -lm $(OBJS4) $(OBJS0) $(OBJS2) -o $@ 

# sms_kernel_test executable
bin/$(TARGET_KT): $(OBJS0) $(OBJS4) $(OBJS3)
	$(CC_DEBUG) $(CFLAGS_DEBUG) -lm $(OBJS4) $(OBJS0) $(OBJS3) -o $@ 

# Bison generates the parser
$(SRC_DIR0)/y.tab.c $(SRC_DIR0)/y.tab.h: $(SRC_DIR1)/sms.y
	bison -dy $(SRC_DIR1)/sms.y -o $(SRC_DIR0)/y.tab.c
	
# Flex generates the lexer
$(SRC_DIR0)/lex.yy.c: $(SRC_DIR0)/y.tab.h $(SRC_DIR0)/y.tab.c $(SRC_DIR1)/sms.l
	flex --yylineno -o $(SRC_DIR0)/lex.yy.c $(SRC_DIR1)/sms.l 

# Compile .o files from .c files
$(BUILD_DIR)/%.c.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compile .dbg.o files from .c files
$(BUILD_DIR)/%.c.dbg.o: %.c
	$(CC_DEBUG) $(CFLAGS_DEBUG) -c $< -o $@

# Reset file state
clean:
	rm -vrf  $(SRC_DIR0)/lex.yy.c\
		$(SRC_DIR0)/y.tab.*\
		$(BUILD_DIR)/$(SRC_DIR0)/*.o\
		$(BUILD_DIR)/$(SRC_DIR1)/*.o\
		$(BUILD_DIR)/$(SRC_DIR2)/*.o\
		$(BUILD_DIR)/$(SRC_DIR3)/*.o\
		bin/sms*
		
# Install the binary to a unix-like system
install: bin/$(TARGET_SMS)
	cp -fv bin/$(TARGET_SMS) $(INSTALL_DIR)/$(TARGET_SMS)
	chmod +x $(INSTALL_DIR)/$(TARGET_SMS)

all: bin/sms bin/sms_test bin/sms_kernel_test