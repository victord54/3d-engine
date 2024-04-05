#!/bin/bash
# This script will run 360 times the cpp executable with the input parameter corresponding to the number of the iteration
for i in {1..360}
do
    ./build/engine $i
done