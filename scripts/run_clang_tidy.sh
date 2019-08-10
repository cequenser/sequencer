#!/bin/bash

COMPILER=$1

cd /home/shared

scripts/ci/install_dependencies_linux.sh ${COMPILER}

mkdir -p build
cd build
cmake .. -DBuildTests=ON -DBuildExamples=ON -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_CXX_COMPILER=${COMPILER}
run-clang-tidy-8.py -header-filter='(include/sequencer|tests).*'
