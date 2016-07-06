#!/bin/bash

# Rebuild
./clean-clang.sh
./build-clang.sh

# Bench
resultsFile=solveSat--clang.csv
find data/uf200-860/ -name uf200-*.cnf -exec sh -c "build-clang/src/solveSat {} | ./perfs/log2csv.awk >> ./perfs/${resultsFile}" \;

