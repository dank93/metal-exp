#! /bin/sh

xcrun -sdk macosx metal \
    -c add_arrays.metal \
    -o add_arrays.air

xcrun -sdk macosx metallib \
    add_arrays.air \
    -o add_arrays.metallib

c++ --std=gnu++20 01_metal_file.cpp \
    -o 01_metal_file.out \
    -I ../../metal_headers/metal-cpp \
    -framework Metal \
    -framework MetalKit \
    -framework Foundation \
