#!/usr/bin/bash
# To be run from top directory
make clean  &&
cd scripts  &&
clang-format --verbose -i ../src/**/*.c ../src/**/*.h ../src/**/**/*.c ../src/**/**/*.h
