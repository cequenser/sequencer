#!/bin/bash

FIXES=${1}
BUILD_URL=${2}
WORK_DIR=${3}

sed 's@build/../include@include@g' ${FIXES} | sed 's@build/../tests@tests@g' | sed "s@^../include@include@g" > adjusted_fixes.yml 2>&1

python2.7 scripts/ci/create_clang_tidy_comments.py adjusted_fixes.yml ${BUILD_URL} > tmp_gerrit_comments.log
sed -i "s/<COMMENTS>/$(sed -e 's/[\&/]/\\&/g' -e 's/$/\\n/' tmp_gerrit_comments.log | tr -d '\n')/" clang-tidy.json

