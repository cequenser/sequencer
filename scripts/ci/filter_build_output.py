#!/usr/bin/python

import os
import re
import sys

def filter_build_output(build_log_name, output_file_name):
    build_log = open(build_log_name, 'r')
    filtered_output = ""
    for line in build_log:
        m = re.match("\[[0-9]+/[0-9]+\] ", line)
        if m:
            continue
        m = re.match("ninja: build stopped:", line)
        if m:
            continue
        filtered_output = filtered_output + line

    output_file = open(output_file_name, 'w')
    output_file.write(filtered_output)


if len(sys.argv) == 3:
    filter_build_output(sys.argv[1], sys.argv[2])
