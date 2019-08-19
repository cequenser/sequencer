#!/bin/bash

LOG_FILE=$1
BUILD_URL=$2
COMPILER=$3

python2.7 filter_build_output.py $LOG_FILE short_build.log

sed 's/"/\\"/g' short_build.log | sed 's@build/../include@include@g' | sed 's@build/../tests@tests@g' | sed "s@^../include@include@g" | sed "s@ ../include@ include@g" | sed "s@^../tests@tests@g" | sed "s@ ../tests@ tests@g" > tmp_comments.log 2>&1
python2.7 create_build_comment.py tmp_comments.log $BUILD_URL $COMPILER > tmp_gerrit_comments.log 2>&1

sed -i "s/<COMMENTS>/$(sed -e 's/[\&/]/\\&/g' -e 's/$/\\n/' tmp_gerrit_comments.log | tr -d '\n')/" tests.json
