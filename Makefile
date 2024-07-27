CC=gcc
CF=-Iinclude -Ofast -Wall -Wno-unused-value -Wno-unused-variable
LDF=-lm -lpthread
DBG=-g3 -fsanitize=address,undefined

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

bin/test_%: $(OBJECT_FILES) test/test_%.c
	@$(CC) $(CF) -o $@ $^

clean:
	@rm -f bin/* obj/*

obj/%.o: src/%.c 
	@echo "[*] Compiling $<"
	@$(CC) -c $(CF) $(DBG) -o $@ $<

$(TARGET): $(OBJECT_FILES)
	@echo "[*] Linking all together"
	@$(CC) -o $@ $^ $(CF) $(DBG) $(LDF)

test: obj/$(U).o tests/test_$(U).c
	@$(CC) -o bin/test_$(U) $^ $(CF)
	@./bin/test_$(U)


.PHONY: all clean run gdb_debug test
