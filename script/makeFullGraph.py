#!/usr/bin/python
import sys
import numpy

n = int(sys.argv[1])
m = n*(n-1)

print n
print m
for i in range(n):
    for j in range(n):
        if i != j:
            print i, j, numpy.random.rand() * 100
