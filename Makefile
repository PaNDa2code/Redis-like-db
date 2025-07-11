CC=gcc
CF=-Iinclude
LDF=-lm -lpthread

MAKEFLAGS+=-j

BIN_DIR=bin
OBJ_DIR=obj
SRC_DIR=src

TARGET=$(BIN_DIR)/Redis

C_FILES=$(wildcard $(SRC_DIR)/*.c)
OBJECT_FILES:=$(subst .c,.o, ${C_FILES})
OBJECT_FILES:=$(subst $(SRC_DIR),$(OBJ_DIR), $(OBJECT_FILES))

mode?=debug

ifeq ($(mode), debug)
	CF+=-g3 -fsanitize=address,undefined -Wall -Wno-unused-value -Wno-unused-variable
else ifeq ($(mode), release)
	# `-flto` is a real magic ^_^
	CF+=-Ofast -s -flto
else
	$?=1
endif

all: print-mode $(OBJ_DIR) $(BIN_DIR) $(TARGET)

print-mode:
	@echo "[!] Build mode: $(mode)"

gdb_debug: $(TARGET)
	gdb $<

clean:
	@rm -f bin/* obj/*

$(OBJ_DIR):
	mkdir $@

$(BIN_DIR):
	mkdir $@

obj/%.o: src/%.c 
	@echo "[*] Compiling $<"
	@$(CC) -c $(CF) -o $@ $<

$(TARGET): $(OBJECT_FILES)
	@echo "[*] Linking all together"
	@$(CC) -o $@ $^ $(CF) $(LDF)

.PHONY: all clean run gdb_debug
