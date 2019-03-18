CC	:= gcc
CFLAGS	:= -Wpedantic -Wall -Wextra # -Werror
LDFLAGS	:= -ll
BUILD	:= ./build
OBJ_DIR	:= $(BUILD)/objects
APP_DIR := $(BUILD)/apps
TARGET	:= slush
INCLUDE	:= -Iinclude/
SRC		:= $(wildcard src/*.c)

OBJECTS	:= $(SRC:%.c=$(OBJ_DIR)/%.o)

# targets
all: build $(APP_DIR)/$(TARGET)

run: all
	$(APP_DIR)/$(TARGET)

flex:
	flex --outfile=src/scanner.c --header-file=include/scanner.h src/scanner.l

bison:
	bison --output=src/parser.c --defines=include/parser.h src/parser.y

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

debug: CFLAGS += -DDEBUG -g
debug: all

release: CFLAGS += -O2
release: all

clean:
	-@rm -rvf $(OBJ_DIR)/*
	-@rm -rvf $(APP_DIR)/*
	-@rm -rvf src/{scanner.c,parser.c} include/{scanner.h,parser.h}
