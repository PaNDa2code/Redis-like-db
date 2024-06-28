#!/bin/bash

gcc -o bin/test src/new_parser.c -Iinclude -DTEST_PARSER

./bin/test

rm bin/test
