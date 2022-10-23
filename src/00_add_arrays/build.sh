#! /bin/sh

c++ --std=gnu++20 00_add_arrays.cpp \
    -I ../../metal_headers/metal-cpp \
    -framework Metal \
    -framework MetalKit \
    -framework Foundation \
