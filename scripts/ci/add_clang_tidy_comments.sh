#!/bin/bash

FIXES=${1}
BUILD_URL=${2}

python2.7 scripts/ci/create_clang_tidy_comments.py ${FIXES} ${BUILD_URL} > tmp_gerrit_comments.log
sed -i "s/<COMMENTS>/$(sed -e 's/[\&/]/\\&/g' -e 's/$/\\n/' tmp_gerrit_comments.log | tr -d '\n')/" clang-tidy.json

