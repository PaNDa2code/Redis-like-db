#!/bin/bash

# Bash script for testing multithread locking, to avoid potential race conditions and ensure reliable testing of locking mechanisms

for i in $(seq 0 1000)
do
  redis-cli SET key$i value$i
done

time (
for i in $(seq 0 1000)
do
  redis-cli GET key$i
done )
