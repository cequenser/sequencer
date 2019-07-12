#!/bin/bash

COMPILER=$1
GCOV=$2

cd /home/shared
mkdir -p build
cd build
rm -rf *

cmake .. -DBuildTests=ON -DCMAKE_CXX_COMPILER=${COMPILER}
if [ -n "${GCOV}" ]; then
    cmake . -DCoverage=ON
fi
cmake --build .

tests/tests

if [ -n "${GCOV}" ]; then
      lcov --gcov-tool ${GCOV} --capture --no-external --directory . --base-directory ../include -rc lcov_branch_coverage=1 --output-file coverage.info
      coveralls-lcov --repo-token ${COVERALLS_TOKEN} coverage.info
fi
