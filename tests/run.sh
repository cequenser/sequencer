#!/bin/bash

COMPILER=$1
COMPUTE_COVERAGE=$2
GCOV=$3

cd /home/shared
mkdir -p build
cd build
rm -rf *

cmake .. -DBuildTests=ON -DCMAKE_CXX_COMPILER=${COMPILER}
if [ "${COMPUTE_COVERAGE}" = "true" ]; then
    cmake . -DCoverage=ON
fi
cmake --build .

tests/tests

if [ "${COMPUTE_COVERAGE}" = "true" ]; then
      lcov --gcov-tool ${GCOV} --capture --no-external --directory . --base-directory ../include -rc lcov_branch_coverage=1 --output-file coverage.info
      coveralls-lcov --repo-token ${COVERALLS_TOKEN} coverage.info
fi
