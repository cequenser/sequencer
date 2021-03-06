#!/bin/bash

CXX_COMPILER=$1
C_COMPILER=$2
GCOV=$3

cd /home/shared

mkdir -p build
cd build
rm -rf *

cmake .. -DSEQUENCER_BuildTests=ON -DSEQUENCER_BuildExamples=ON -DCMAKE_CXX_COMPILER=${CXX_COMPILER} -DCMAKE_C_COMPILER=${C_COMPILER} -DSEQUENCER_DisableAssert=ON
if [ -n "${GCOV}" ]; then
    cmake . -DSEQUENCER_Coverage=ON
fi
cmake --build .
cmake --build . --target test

if [ -n "${GCOV}" ]; then
      lcov --gcov-tool ${GCOV} --capture --no-external --directory . --base-directory ../include -rc lcov_branch_coverage=1 --output-file coverage.info
      coveralls-lcov --repo-token ${COVERALLS_TOKEN} coverage.info
fi
