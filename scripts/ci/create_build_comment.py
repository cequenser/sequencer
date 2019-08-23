#!/usr/bin/python

import os
import re
import sys


indent = '  '


current_line = None
last_line = None
second_last_line = None


def readline(log):
    global current_line
    global last_line
    global second_last_line

    line = log.readline()
    if last_line:
        second_last_line = last_line
    if current_line:
        last_line = current_line
    current_line = line
    return log


def read_error_location(filename, linenumber):
    with open(filename) as fp:
        for i, line in enumerate(fp):
            if i == int(linenumber) - 1:
                return line
    return None

def create_build_comment_for_gcc(log_filename, robot_run_id, compiler):
    global current_line
    global last_line
    global second_last_line

    build_log = open(log_filename, 'r')
    build_log = readline(build_log)
    while current_line:
        m = re.match(r'(\S+):([0-9]+):([0-9]+): error:.*', current_line)

        # error
        if m:
            filename = m.group(1)
            linenumber = m.group(2)
            pos = int(m.group(3))

            message = ""
            if second_last_line and re.match(r'instantiation of', second_last_line):
                message += second_last_line[line.find(':')+2:len(line)-1] + ' requested here\n'

            if last_line:
                m = re.match(filename + ':([0-9]+):([0-9]+):\s+required from here', last_line)
                if m:
                    linenumber = m.group(1)
                    pos = int(m.group(2))
                    message += read_error_location(filename, linenumber)
                    message += ' ' * (pos-1) + '^\n\n'

            message += current_line
            build_log = readline(build_log)
            message += current_line
            build_log = readline(build_log)
            message += current_line

            # gerrit comment
            print indent*1 + '"' + filename + '": ['
            print indent*3 + '{'
            print indent*4 + '"robot_id": "' + compiler + '",'
            print indent*4 + '"robot_run_id": "' + robot_run_id + '",' 
            print indent*4 + '"line": "' + linenumber + '",' 
            print indent*4 + '"message": "' + message + '"' 
            print indent*3 + '}' 
            print indent*2 + ']'
            return


def create_build_comment_for_clang(log_filename, robot_run_id, compiler):
    build_log = open(log_filename, 'r')
    line = build_log.readline()
    line = build_log.readline()
    m = re.match('^\S*clang\+\+.* (\S+\.cpp)', line)
    if not m:
        return
    filename = m.groups()[0]

    # actual error
    line = build_log.readline()
    while line:
        m = re.match(r'\S+:[0-9]+:[0-9]+: error:.*', line)
        if m:
            error_message = line
            line = build_log.readline()
            error_message += line
            line = build_log.readline()
            error_message += line
            break
        line = build_log.readline()

    # place of error
    line = build_log.readline()
    m = re.match(filename + ':([0-9]+):[0-9]+:\s+note:.*', line)
    if not m:
        return
    message = line[line.find('note:')+5:]
    message += build_log.readline()
    message += build_log.readline()
    message += '\n' + error_message

    # gerrit comment
    print indent*1 + '"' + filename + '": ['
    print indent*3 + '{'
    print indent*4 + '"robot_id": "' + compiler + '",'
    print indent*4 + '"robot_run_id": "' + robot_run_id + '",' 
    print indent*4 + '"line": "' + m.group(1) + '",' 
    print indent*4 + '"message": "' + message + '"' 
    print indent*3 + '}' 
    print indent*2 + ']'


def create_build_comment(log_filename, robot_run_id, compiler):
    if compiler.startswith('g++'):
        create_build_comment_for_gcc(log_filename, robot_run_id, compiler)
    else:
        create_build_comment_for_clang(log_filename, robot_run_id, compiler)

if len(sys.argv) == 4:
    create_build_comment(sys.argv[1], sys.argv[2], sys.argv[3])
