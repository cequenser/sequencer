#!/bin/bash

LOG_FILE=$1
BUILD_URL=$2
WORK_DIR=$3

PLACEHOLDER="__NEWLINE__"
sed 's@build/../include@include@g' $LOGFILE | sed -e ':a' -e 'N' -e '$!ba' -e "s/\n/${PLACEHOLDER}/g" | sed "s/${PLACEHOLDER}${PLACEHOLDER}/${PLACEHOLDER}\n/g" | sed "s@${PLACEHOLDER}${4}@${PLACEHOLDER}\n${4}@g"  | sort -u > tmp_comments.log 2>&1

cat tmp_comments.log

python2.7 create_clang_tidy_comments.py tmp_comments.log ${PLACEHOLDER} clang-tidy $BUILD_URL $WORK_DIR > tmp_gerrit_comments.log 2>&1

cat tmp_gerrit_comments.log

sed -i "s/<COMMENTS>/$(sed -e 's/[\&/]/\\&/g' -e 's/$/\\n/' tmp_gerrit_comments.log | tr -d '\n')/" clang-tidy.json
