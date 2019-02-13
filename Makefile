CC	:= gcc
CFLAGS	:= -Wpedantic -Wall -Werror -Wextra
# LDFLAGS	:= -L/usr/lib -lm
BUILD	:= ./build
OBJ_DIR	:= $(BUILD)/objects
APP_DIR := $(BUILD)/apps
TARGET	:= slush
INCLUDE	:= -Iinclude/
# SRC	:=			\
#	$(wildcard src/*.c)
SRC	:= src/main.c

OBJECTS	:= $(SRC:%.c=$(OBJ_DIR)/%.o)

# targets
all: build $(APP_DIR)/$(TARGET)

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $<

$(APP_DIR)/$(TARGET): $(OBJECTS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDE) -o $(APP_DIR)/$(TARGET) $(OBJECTS)
# $(CC) $(CFLAGS) $(INCLUDE) $(LDFLAGS) -o $(APP_DIR)/$(TARGET) $(OBJECTS)

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
