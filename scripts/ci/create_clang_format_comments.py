#!/usr/bin/python

import os
import re
import sys

indent = '  '


def extract_filename_dictionary_clang_format(clang_filename):
    clang_file = open(clang_filename, 'r')
    comments = {}

    filename = ""
    message = ""
    for line in clang_file:
        if line.startswith('---'):
            if not message == "":
                comments[filename] = message
                message = ""
            m = re.match("--- (\S+).*", line)
            if m:
                filename = m.groups()[0]
            continue
        if line.startswith('+++') or line.startswith('@@'):
            continue
        message = message + line

    comments[filename] = message
    return comments


def create_comment_section_clang_format(comments, robot_id, robot_run_id):
    comment_section = ""
    first_filename = True
    for filename, message in comments.items():
        if not first_filename:
            comment_section = comment_section + ',\n'
        first_filename = False
        comment_section = comment_section + '"' + filename + '": [\n'
        start_comment = indent*3 +'{\n'
        comment_body = indent*4 + '"robot_id": "' + robot_id + '",\n'
        comment_body = comment_body + indent*4 + '"robot_run_id": "' + robot_run_id + '",\n'
        comment_body = comment_body + indent*4 + '"message": "' + message + '"\n'
        end_comment = indent*3 + '}'
        comment_section = comment_section + start_comment + comment_body + end_comment
        comment_section = comment_section + '\n' + indent*2 + ']'
    comment_section = comment_section
    return comment_section


def create__clang_format_comments(clang_filename, robot_id, robot_run_id):
    comments = extract_filename_dictionary_clang_format(clang_filename)
    comment_section = create_comment_section_clang_format(comments, robot_id, robot_run_id)
    print comment_section


if len(sys.argv) == 4:
    create__clang_format_comments(sys.argv[1], sys.argv[2], sys.argv[3])
