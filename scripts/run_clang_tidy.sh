#!/bin/bash

cd /home/shared
mkdir -p build
cd build

cmake .. -DBuildTests=ON -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
run-clang-tidy-8.py -header-filter=.*
