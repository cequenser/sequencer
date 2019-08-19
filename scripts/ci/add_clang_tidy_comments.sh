#!/bin/bash

LOG_FILE=$1
BUILD_URL=$2
WORK_DIR=$3

sed 's@build/../include@include@g' $LOG_FILE | sed 's@build/../tests@tests@g' | sed "s@^../include@include@g" | sed "s@^include@${WORK_DIR}include@g" | sed "s@^tests@${WORK_DIR}tests@g" > tmp_comments.log 2>&1

python2.7 create_clang_tidy_comments.py tmp_comments.log clang-tidy $BUILD_URL $WORK_DIR > tmp_gerrit_comments.log 2>&1

sed -i "s/<COMMENTS>/$(sed -e 's/[\&/]/\\&/g' -e 's/$/\\n/' tmp_gerrit_comments.log | tr -d '\n')/" clang-tidy.json
