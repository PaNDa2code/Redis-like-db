#!/bin/bash

for i in $(seq 1 10)
do 
  echo "SET key$i value$i" | redis-cli 
done
