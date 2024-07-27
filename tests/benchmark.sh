#!/bin/bash

(yes | ./bin/MyRides -p 123 1> /dev/null) & 

SERVER_PID=$(pgrep -f "./bin/MyRides")

redis-benchmark -t SET,GET -n 100000 -r 1000000000000 -c 100 -q --threads 5 -p 123

kill -2 $SERVER_PID
