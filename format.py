#!/usr/bin/python

import sys
import os


ldc =[]
for line in file(sys.argv[1], "r"):
	index , count = line.strip().split()
	ldc.append((index, int(count)))

vec = []
for line in file(sys.argv[2], "r"):
	vec.append(line.strip())

index = 0
for x in ldc:
	for i in xrange(x[1]):
		print "%s %s" % (x[0], vec[index])
		index += 1

