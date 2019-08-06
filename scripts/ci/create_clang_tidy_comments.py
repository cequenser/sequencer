#!/usr/bin/python

import os
import re
import sys

indent = '  '

def extract_filename_dictionary_clang_tidy(clang_filename, newline_placeholder, base_path):
    clang_file = open(clang_filename, 'r')
    comments = {}
    for line in clang_file:
        print line
        print base_path
        if not line.startswith(base_path) and not line.startswith('../include'):
            continue
        firstColon = line.find(':')
        filename = line[len(base_path):firstColon]
        line_number = line[firstColon+1:line.find(':', firstColon+1)]
        message_start = line.find('error:')+7
        message_end = max(line.find(base_path, message_start), line.find('Suppressed', message_start))
        m = re.match(".*(" + newline_placeholder + "\s*[0-9]+\s*warnings\s+generated.).*", line[message_start:])
        if m:
            message_end = line.find(m.groups()[0], message_start)
        message = line[message_start:message_end].replace(newline_placeholder, '\n')

        if filename in comments.keys():
            if not [line_number, message] in comments[filename]:
                comments[filename].append([line_number, message])
        else:
            comments[filename] = [[line_number, message]]
    return comments


def create_comment_section_clang_tidy(comments, robot_id, robot_run_id):
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


def create_clang_tidy_comments(clang_filename, newline_placeholder, robot_id, robot_run_id, base_path):
    comments = extract_filename_dictionary_clang_tidy(clang_filename, newline_placeholder, base_path)
    comment_section = create_comment_section_clang_tidy(comments, robot_id, robot_run_id)
    print comment_section


if len(sys.argv) == 6:
    create_clang_tidy_comments(sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4], sys.argv[5])
