#! /bin/sh

c++ --std=gnu++20 00_add_arrays.cpp \
    -o 00_add_arrays.out \
    -I ../../metal_headers/metal-cpp \
    -framework Metal \
    -framework MetalKit \
    -framework Foundation \
