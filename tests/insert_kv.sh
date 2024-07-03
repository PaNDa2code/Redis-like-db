#!/bin/bash

SEQ=$(seq 0 10)

for i in $SEQ
do 
  echo "SET key$i value$i" | redis-cli 
done

for i in $SEQ
do 
  echo "GET key$i" | redis-cli
done

echo "SET keyPX valuePX PX 2000" | redis-cli 

echo "GET keyPX" | redis-cli 

sleep 2

echo "GET keyPX" | redis-cli 
