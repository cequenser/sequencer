#!/bin/bash

CXX_COMPILER=$1
C_COMPILER=$2

cd /home/shared

scripts/ci/install_dependencies_linux.sh ${CXX_COMPILER} ${C_COMPILER}

mkdir -p build
cd build
cmake .. -DBuildTests=ON -DBuildExamples=ON -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_CXX_COMPILER=${CXX_COMPILER}
run-clang-tidy-8.py -header-filter='(include/sequencer|tests).*'
