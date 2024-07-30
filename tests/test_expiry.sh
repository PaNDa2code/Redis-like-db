#!/bin/bash

for ((i = 1; i <= 10; i++)); do
  printf "SET key%d value PX %d000\n" "$i" "$i" | redis-cli
done

for ((i = 1; i <= 10; i++)); do
  printf "SET key%d value PX %d000\n" "$i" "$i" | redis-cli
done
