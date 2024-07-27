#!/bin/bash

gcc -o test tests/src/test_lexer.c src/parse_command.c -Iinclude

./test 

rm test
