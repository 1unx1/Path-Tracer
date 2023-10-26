#!/usr/bin/env bash
SAMPLES=100
DEPTH=20
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

time build/PA1 testcases/ball.txt output/ball.bmp $SAMPLES $DEPTH
time build/PA1 testcases/global.txt output/global.bmp $SAMPLES $DEPTH

time build/PA1 testcases/ball_texture.txt output/ball_texture.bmp $SAMPLES $DEPTH
time build/PA1 testcases/ball_moving.txt output/ball_moving.bmp $SAMPLES $DEPTH

time build/PA1 testcases/ball_dof.txt output/ball_dof.bmp $SAMPLES $DEPTH

time build/PA1 testcases/bunny_1k.txt output/bunny_1k.bmp $SAMPLES $DEPTH
time build/PA1 testcases/bunny_1k_vn.txt output/bunny_1k_vn.bmp $SAMPLES $DEPTH

time build/PA1 testcases/revsurface.txt output/revsurface.bmp $SAMPLES $DEPTH
