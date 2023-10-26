#!/usr/bin/env bash
SAMPLES=300
DEPTH=15
# If project not ready, generate cmake file.
if [[ ! -d build ]]; then
    mkdir -p build
    cd build
    cmake ..
    cd ..
fi

# Build project.
cd build
make -j
cd ..
# pt

time build/PA1 testcases/final.txt output/final.bmp $SAMPLES $DEPTH