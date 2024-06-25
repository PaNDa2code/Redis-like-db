#!/bin/bash

gcc -o bin/test src/parser.c -Iinclude -DTEST_PARSER

./bin/test & 

PID=$!

wait $PID 

rm bin/test
