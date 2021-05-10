#!/bin/bash

perl -pe 's/((?<=\t)|(?<=^))\t/ \t/g;' "$@" | column -t -s $'\t' | exec less  -F -S -X -K

