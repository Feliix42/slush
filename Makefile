CC	:= gcc
YACC	:= bison
LEX	:= flex
CFLAGS	:= -std=c11 -D_XOPEN_SOURCE=500 -Wpedantic -Wall -Wextra # -Werror
# LDFLAGS	:= -ll
BUILD	:= ./build
OBJ_DIR	:= $(BUILD)/objects
APP_DIR := $(BUILD)/apps
TARGET	:= slush
INCLUDE	:= -Iinclude/
SRC	:= $(wildcard src/*.c)

# use libedit on macOS
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
        LDFLAGS += -ledit -ltermcap -ll
else
        LDFLAGS += -lreadline
endif

OBJECTS	:= $(SRC:%.c=$(OBJ_DIR)/%.o)

# targets
all: build $(APP_DIR)/$(TARGET)

run: all
	$(APP_DIR)/$(TARGET)

flex:
	$(LEX) --outfile=src/scanner.c --header-file=include/scanner.h src/scanner.l

bison:
	$(YACC) --output=src/parser.c --defines=include/parser.h src/parser.y

generate: flex bison

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $<

$(APP_DIR)/$(TARGET): flex bison $(OBJECTS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDE) $(LDFLAGS) -o $(APP_DIR)/$(TARGET) $(OBJECTS)

.PHONY: all build clean debug release

build:
	@mkdir -p $(APP_DIR)
	@mkdir -p $(OBJ_DIR)

debug: CFLAGS += -DDEBUG -g -fsanitize=address
debug: all

release: CFLAGS += -O2
release: all

clean:
	-@rm -rvf $(OBJ_DIR)/*
	-@rm -rvf $(APP_DIR)/*
	-@rm -rvf src/{scanner.c,parser.c} include/{scanner.h,parser.h}
