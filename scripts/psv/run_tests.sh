#!/bin/bash

COMPILER=$1

mkdir -p build
cd build
rm -rf *

cmake .. -GNinja -DBuildTests=ON -DCMAKE_CXX_COMPILER=${COMPILER}
cmake --build .

tests/tests

