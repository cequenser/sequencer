#!/usr/bin/python

import os
import re
import sys
from copy import deepcopy

indent = '  '


DIAGNOSTIC_NAME = 'DiagnosticName'
FILE_OFFSET = 'FileOffset'
FILE_PATH = 'FilePath'
MESSAGE = 'Message'
REPLACEMENTS = 'Replacements'
LENGTH = 'Length'
OFFSET = 'Offset'
REPLACEMENT_TEXT = 'ReplacementText'


class HashableDict(dict):
    def __hash__(self):
        return hash(tuple(sorted(self.items())))


def read_replacements(line, clang_file):
    while REPLACEMENT_TEXT not in line:
        line = clang_file.readline()
    return line[line.find(REPLACEMENT_TEXT) + len(REPLACEMENT_TEXT) + 2:line.find('}')], line


def read_diagnostics(clang_filename):
    clang_file = open(clang_filename, 'r')
    diagnostics = {}
    clang_file.readline()
    while True:
        line = clang_file.readline()
        if not line.startswith('- ' + DIAGNOSTIC_NAME):
            break
        diagnostic = HashableDict()
        diagnostic[DIAGNOSTIC_NAME] = line[len(DIAGNOSTIC_NAME) + 4:-1]
        line = clang_file.readline()
        diagnostic[FILE_OFFSET] = int(line[len(FILE_OFFSET) + 4:-1])
        line = clang_file.readline()
        file_path = line[len(FILE_PATH) + 4:-1]
        line = clang_file.readline()
        diagnostic[MESSAGE] = line[len(MESSAGE) + 4:-1]
        line = clang_file.readline()
        if REPLACEMENTS + ': []' in line:
            diagnostic[REPLACEMENT_TEXT] = None
        else:
            line = clang_file.readline()
            diagnostic[REPLACEMENT_TEXT], line = read_replacements(line, clang_file)
        if not diagnostics.get(file_path):
            diagnostics[file_path] = set()
        diagnostics[file_path].add(diagnostic)
    return diagnostics


def get_message(diagnostic, line, offset):
    indent = ' ' * offset
    message = 'error: ' + diagnostic[DIAGNOSTIC_NAME] + '\n' + line + indent + '^'
    if diagnostic.get(REPLACEMENT_TEXT):
        message += ('~' * (len(diagnostic[REPLACEMENT_TEXT]) - 1)) + '\n'
        message += indent + diagnostic[REPLACEMENT_TEXT]
    return message


def get_line_and_message(diagnostic, filename):
    file_ = open(filename, 'r')
    offset = deepcopy(diagnostic[FILE_OFFSET])
    line_number = -1
    for line in file_:
        line_number += 1
        if offset - len(line) < 0:
            return line_number, get_message(diagnostic, line, offset)
        offset -= len(line)
    return None, None


def write_comments(diagnostics, robot_id, robot_run_id):
    comment_section = ""
    first_filename = True
    for filename, diagnostic_set in diagnostics.items():
        if not first_filename:
            comment_section = comment_section + ',\n'
        first_filename = False
        comment_section = comment_section + '"' + filename + '": [\n'
        first_comment = True

        for diagnostic in diagnostic_set:
            line_number, message = get_line_and_message(diagnostic, filename)
            if not first_comment:
                comment_section = comment_section + ',\n'
            first_comment = False
            start_comment = indent*3 +'{\n'
            comment_body = indent*4 + '"robot_id": "' + robot_id + '",\n'
            comment_body = comment_body + indent*4 + '"robot_run_id": "' + robot_run_id + '",\n'
            comment_body = comment_body + indent*4 + '"line": "' + str(line_number) + '",\n'
            comment_body = comment_body + indent*4 + '"message": "' + message + '"\n'
            end_comment = indent*3 + '}'
            comment_section = comment_section + start_comment + comment_body + end_comment
        comment_section = comment_section + '\n' + indent*2 + ']'
    comment_section = comment_section
    return comment_section


def create_clang_tidy_comments(clang_filename, robot_run_id):
    diagnostics = read_diagnostics(clang_filename)
    comments = write_comments(diagnostics=diagnostics, robot_id='clang-tidy', robot_run_id=robot_run_id)
    print comments


if len(sys.argv) == 3:
    create_clang_tidy_comments(sys.argv[1], sys.argv[2])
