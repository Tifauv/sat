#!/bin/bash

# Rebuild
./clean.sh
./build.sh

# Bench
resultsFile=solveSat--gcc.csv
find data/uf200-860/ -name uf200-*.cnf -exec sh -c "build/src/solveSat {} | ./perfs/log2csv.awk >> ./perfs/${resultsFile}" \;

