CC=gcc
CF=-g -Iinclude -O3
TARGET=bin/MyRides
OBJ_DIR=obj/
SRC_DIR=src/

C_FILES=$(wildcard $(SRC_DIR)*.c)
OBJECT_FILES:=$(subst .c,.o, ${C_FILES})
OBJECT_FILES:=$(subst $(SRC_DIR),$(OBJ_DIR), $(OBJECT_FILES))

all: $(TARGET)

run: $(C_FILES)
	@$(CC) $(CF) -o bin/MyRides $^
	@./bin/MyRides

gdb_debug: $(C_FILES)
	@$(CC) $(CF) -g -o bin/MyRides $^
	gdb bin/MyRides

clean:
	@rm -f bin/* obj/*

.PHONY: all clean run gdb_debug

obj/%.o: src/%.c 
	@echo "[*] Compiling $<"
	@$(CC) -c $(CF) -o $@ $<

$(TARGET): $(OBJECT_FILES)
	@echo "[*] Linking all together"
	@$(CC) $(CF) -o $@ $^
