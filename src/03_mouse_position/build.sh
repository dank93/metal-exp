#! /bin/sh

c++ --std=gnu++20 03_mouse_position.cpp \
    -o 03_mouse_position.out \
    -I ../../metal_headers/metal-cpp \
    -I ../../metal_headers/metal-cpp-extensions \
    -framework Foundation \
    -framework AppKit \
    -framework Metal \
    -framework MetalKit
