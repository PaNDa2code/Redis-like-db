#!/bin/python3

import redis
import time
from tqdm import tqdm

r = redis.Redis(host='localhost', port=6379, db=0)

def test_expiry_data_cleanup(i):
    key = f'test_key{i}'
    r.set(key, 'test_value', px=250)
    assert r.get(key) == b'test_value', "Key should exist and have the correct value"
    time.sleep(0.25)
    assert r.get(key) is None, "Key should be expired and deleted"

if __name__ == "__main__":
    for i in tqdm(range(20)):
        test_expiry_data_cleanup(i)

