#!/bin/bash

python2.7 create_clang_format_comments.py clang-format.log clang-format $1 > tmp_gerrit_comments.log 2>&1

sed -i "s/<COMMENTS>/$(sed -e 's/[\&/]/\\&/g' -e 's/$/\\n/' tmp_gerrit_comments.log | tr -d '\n')/" clang-format.json
