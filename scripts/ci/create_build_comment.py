#!/usr/bin/python

import os
import re
import sys


indent = '  '


def create_build_comment(log_filename, robot_run_id, compiler):
    build_log = open(log_filename, 'r')
    line = build_log.readline()
    line = build_log.readline()
    m = re.match('^\S*g\+\+.* (\S+\.cpp)', line)
    if not m:
        return
    filename = m.groups()[0]

    line = build_log.readline()
    # in instantiation of ...
    line = build_log.readline()
    message = line[line.find(':')+2:]

    # place of error
    line = build_log.readline()
    m = re.match(filename + ':([0-9]+):([0-9]+):\s+required from here', line)
    if not m:
        return
    linenumber = m.group(1)
    pos = int(m.group(2))
    with open(filename) as fp:
        for i, line in enumerate(fp):
            if i == int(linenumber):
                message_start = line
            elif i > linenumber:
                break
    message_start += ' ' * pos + '^' + '~'*20

    # actual error
    line = build_log.readline()
    while line:
        m = re.match(r'\S+:[0-9]+:[0-9]+: error:.*', line)
        if m:
            message += line
            line = build_log.readline()
            message += line
            line = build_log.readline()
            message += line
            break
        line = build_log.readlin()

    # gerrit comment
    print indent*2 + '"' + filename + '": ['
    print indent*3 + '{'
    print indent*4 + '"robot_id": "' + compiler + '",'
    print indent*4 + '"robot_run_id": "' + robot_run_id + '",' 
    print indent*4 + '"line": "' + linenumber + '",' 
    print indent*4 + '"message": "' + message_start +'\n' + message + '"' 
    print indent*3 + '}' 
    

if len(sys.argv) == 4:
    create_build_comment(sys.argv[1], sys.argv[2], sys.argv[3])
