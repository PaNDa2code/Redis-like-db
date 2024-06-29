#!/bin/bash

# make clean
# make 
# sleep 2 && ./tests/set_test_data.sh &
# valgrind --leak-check=full ./bin/MyRides

make clean 
make CF="-lm -g -Iinclude -O3 -fsanitize=address"

./bin/MyRides
