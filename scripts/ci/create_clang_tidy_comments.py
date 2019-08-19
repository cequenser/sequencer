#!/usr/bin/python

import os
import re
import sys

indent = '  '


def get_message_start_index(line):
    message_start = line.find('error:') + 7
    if message_start == -1:
        message_start = line.find('note:') + 6
    return message_start


def get_filename_and_linenumber(line, base_path = None):
    if base_path:
        m = re.match(r'^' + base_path + r'(\S*):([0-9]+):[0-9]+:', line)
    else:
        m = re.match(r'^(\S+):([0-9]+):[0-9]+:', line)
    if m:
         return m.groups()[0], m.groups()[1]
    return None, None    


def read_message(clang_file, line, base_path):
    if not line.startswith(base_path):
        line = clang_file.readline()
        return line, None, None

    filename, linenumber = get_filename_and_linenumber(line, base_path)
    message = line[get_message_start_index(line):]

    line = clang_file.readline()
    while line and not line in ['\n', '\r\n'] and not "warnings generated" in line:
	message += line
        line = clang_file.readline()
        other_filename, other_linenumber = get_filename_and_linenumber(line)
        if other_filename and other_filename != filename:
            break

    if "warnings generated" in line:
        line = clang_file.readline()

#    print " === \n" + message + "\n === "
    return line, filename, [linenumber, message]


def read_messages(clang_file, line, base_path, comments):
    while line and not line.startswith('clang-tidy'):
        line, filename, message = read_message(clang_file, line, base_path)
        if filename:
            if filename in comments.keys():
                comments[filename].append(message)
            else:
                comments[filename] = [message]
    return comments, line


def extract_filename_dictionary(filename, base_path):
    clang_file = open(filename, 'r')
    comments = {}

    linenumber = -1
    message = ""

    line = clang_file.readline()

    while line:
	if line.startswith('clang-tidy'):
            line = clang_file.readline()
            comments, line = read_messages(clang_file, line, base_path, comments)               
        else:
            line = clang_file.readline()
    return comments        


def create_comment_section(comments, robot_id, robot_run_id):
    comment_section = ""
    first_filename = True
    for filename, comment in comments.items():
        if not first_filename:
            comment_section = comment_section + ',\n'
        first_filename = False
        comment_section = comment_section + '"' + filename + '": [\n'
        first_comment = True
        for line_number, message in comment:
            if not first_comment:
                comment_section = comment_section + ',\n'
            first_comment = False
            start_comment = indent*3 +'{\n'
            comment_body = indent*4 + '"robot_id": "' + robot_id + '",\n'
            comment_body = comment_body + indent*4 + '"robot_run_id": "' + robot_run_id + '",\n'
            comment_body = comment_body + indent*4 + '"line": "' + line_number + '",\n'
            comment_body = comment_body + indent*4 + '"message": "' + message + '"\n'
            end_comment = indent*3 + '}'
            comment_section = comment_section + start_comment + comment_body + end_comment
        comment_section = comment_section + '\n' + indent*2 + ']'
    comment_section = comment_section
    return comment_section


def create_clang_tidy_comments(clang_filename, robot_id, robot_run_id, base_path):
    comments = extract_filename_dictionary(clang_filename, base_path)
    comment_section = create_comment_section(comments, robot_id, robot_run_id)
    print comment_section


if len(sys.argv) == 5:
    create_clang_tidy_comments(sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4])
