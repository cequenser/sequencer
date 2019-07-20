#!/bin/bash

PLACEHOLDER="__NEWLINE__"
sed 's@build/../include@include@g' $1 | sed -e ':a' -e 'N' -e '$!ba' -e "s/\n/${PLACEHOLDER}/g" | sed "s/${PLACEHOLDER}${PLACEHOLDER}/${PLACEHOLDER}\n/g" | sed "s@${PLACEHOLDER}${4}@${PLACEHOLDER}\n${4}@g"  | sort -u > tmp_comments.log 2>&1

python2.7 create_comments.py tmp_comments.log ${PLACEHOLDER} $2 $3 $4 > tmp_gerrit_comments.log 2>&1

sed -i "s/<COMMENTS>/$(sed -e 's/[\&/]/\\&/g' -e 's/$/\\n/' tmp_gerrit_comments.log | tr -d '\n')/" $2.json
