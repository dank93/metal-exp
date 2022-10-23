#! /bin/sh

c++ --std=gnu++20 02_app_window.cpp \
    -o 02_app_window.out \
    -I ../../metal_headers/metal-cpp \
    -I ../../metal_headers/metal-cpp-extensions \
    -framework Foundation \
    -framework AppKit \
    -framework Metal \
    -framework MetalKit
