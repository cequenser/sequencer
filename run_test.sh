#!/bin/bash

cd /home/shared
mkdir -p build
cd build
rm -rf *

cmake .. -DBuildTests=ON -DCMAKE_CXX_COMPILER=$1
cmake --build .

tests/tests
