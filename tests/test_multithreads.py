#!/bin/python3

import redis
from random import randbytes
from tqdm import tqdm
from multiprocessing import Process

def test_multi_threading_write():
    with redis.Redis(host="localhost", port=6379, db=0) as r:
        for _ in range(500):
            key = randbytes(10).hex()
            value = randbytes(10).hex()
            r.set(key, value, px=100)
            assert r.get(key) != value
            r.set(key, value, px=0)
            assert r.get(key) != None

if __name__ == "__main__":
    processes = []
    for _ in range(100):
        process = Process(target=test_multi_threading_write)
        processes.append(process)
        process.start()
    for index in tqdm(range(len(processes))):
        processes[index].join()
