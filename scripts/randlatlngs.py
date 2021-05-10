#!/usr/bin/python3

import os, sys, random

infile = sys.stdin if len(sys.argv) < 2 else open(sys.argv[1], "r")

# http://en.wikipedia.org/wiki/Extreme_points_of_the_United_States#Westernmost
north = 49.3457868
south = 24.7433195
east = -66.9513812
west = -124.7844079

for line in infile.readlines():
    sample = line.replace("\n", "")
    lat = random.uniform(south, north)
    lng = random.uniform(west, east)
    print(f"{sample} {lat} {lng}")
