#!/bin/bash

CXX_COMPILER=$1
C_COMPILER=$2
GCOV=$3

cd /home/shared

mkdir -p build
cd build
rm -rf *

cmake .. -DBuildTests=ON -DBuildExamples=ON -DCMAKE_CXX_COMPILER=${CXX_COMPILER} -DDisableAssert=ON
if [ -n "${GCOV}" ]; then
    cmake . -DCoverage=ON
fi
cmake --build .
cmake --build . --target test

if [ -n "${GCOV}" ]; then
      lcov --gcov-tool ${GCOV} --capture --no-external --directory . --base-directory ../include -rc lcov_branch_coverage=1 --output-file coverage.info
      coveralls-lcov --repo-token ${COVERALLS_TOKEN} coverage.info
fi
