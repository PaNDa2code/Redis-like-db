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

#
# echo "GET key0" | redis-cli &
# echo "GET key1" | redis-cli &
# echo "GET key2" | redis-cli &
# echo "GET key3" | redis-cli &
# echo "GET key4" | redis-cli
#
#
# echo "SET key11 value0" | redis-cli &
# echo "SET key12 value0" | redis-cli &
# echo "SET key13 value0" | redis-cli &
# echo "SET key14 value0" | redis-cli &
# echo "SET key15 value0" | redis-cli &
# echo "SET key16 value0" | redis-cli
