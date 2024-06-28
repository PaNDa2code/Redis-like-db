#!/bin/bash

# Bash script for testing multithread locking, to avoid potential race conditions and ensure reliable testing of locking mechanisms

redis-cli SET key0 value0 &&
redis-cli SET key1 value1 && 

redis-cli SET key2 value2 && 
redis-cli SET key3 value3 # && 

# redis-cli SET key4 value4 &
# redis-cli SET key5 value5 

