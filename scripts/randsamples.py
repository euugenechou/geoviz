#!/usr/bin/python3

import os, random, sys

if len(sys.argv) != 2:
    print(f"Usage: {sys.argv[0]} [number of samples]")

# http://en.wikipedia.org/wiki/Extreme_points_of_the_United_States#Westernmost
north = 49.3457868
south = 24.7433195
east = -66.9513812
west = -124.7844079

# Generate and print random samples.
for i in range(1, int(sys.argv[1]) + 1):
    sample = f"fake_sample_{i}"
    lat = random.uniform(south, north)
    lng = random.uniform(west, east)
    print(f"{sample} {lat} {lng}")
