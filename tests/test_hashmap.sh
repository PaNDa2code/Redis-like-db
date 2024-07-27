gcc -g -o test tests/src/test_hashmap.c src/hashmap.c src/str_hash.c -Iinclude
valgrind --leak-check=full ./test 
# gdb ./test
# rm test
