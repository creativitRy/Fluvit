#!/usr/bin/env python3

import re
import sys
import os
import argparse
import colorama
from colorama import Fore, Style
from zipfile import ZipFile
import tarfile

def _msg(text, level, color):
    return Style.BRIGHT+color+level+Fore.RESET+Style.NORMAL+text

def _dir(text):
    index = text.rfind('/')
    return text[:index]

def try_zip(fn):
    try:
        with ZipFile(fn, 'r') as zipf:
            return zipf.namelist()
    except:
        return None

def try_tar(fn):
    try:
        with tarfile.open(fn, 'r:*') as tar:
            return tar.getnames()
    except:
        return None

ARCHIVE_HANDLERS = [try_zip, try_tar]

CRITERIA = [
        (re.compile('.*\.git/'), 'Git repository at {adir} is not removed'),
        (re.compile('.*/CMakeCache\.txt$'), 'Build directory {adir} is not removed'),
        (re.compile('.*\.o$'), 'Object file {afile} is not removed'),
        (re.compile('.*\.ray$'), 'Ray file at {afile} is not removed'),
        (re.compile('.*\.bmp$'), 'Image file at {afile} is not removed'),
        (re.compile('.*\.pmd$'), 'PMD file at {afile} is not removed'),
        (re.compile('.*\.jp[e]?g$'), 'Image file at {afile} is not removed'),
        (re.compile('.*\._.*'), 'macOS ._ file at {afile} is not removed'),
            ]

class ArchiveChecker:
    fn = None
    files = None
    problems = None

    def __init__(self,
            fn):
        self.problems = []
        for handler in ARCHIVE_HANDLERS:
            files = handler(fn)
            if files is not None:
                files.sort()
                self.files = files
                break
        self.fn = fn

        self.guess_project_name()

    def crit(self, text):
        self.problems.append(_msg(text, '[CRITICAL] ', Fore.RED))

    def bug(self, text):
        self.problems.append(_msg(text, '[WARNING] ', Fore.YELLOW))

    def good(self, text):
        self.problems.append(_msg(text, '[GOOD] ', Fore.GREEN))

    def collect_problems(self):
        if self.files is None:
            self.crit('Cannot open {file} as zip or tar')
            return
        '''
        Filtering out junk files
        '''
        for tup in CRITERIA:
            matched = next((f for f in self.files if tup[0].match(f)), None)
            if matched is not None:
                self.bug(tup[1].format(afile=matched, adir=_dir(matched)))
        '''
        Stop tar bomb
        '''
        fn_with_dir = next((f for f in self.files if '/' in f), None)
        if fn_with_dir is None:
            self.crit('Files should be contained by a directory')
        else:
            root_dir = fn_with_dir.split('/')[0]
            if self.project_name is not None and root_dir != self.project_name:
                self.bug('The root directory should be {}, not {}'.format(self.project_name, root_dir))
            for f in self.files:
                segs = f.split('/')
                another = segs[0]
                if another != root_dir:
                    if len(segs) == 1:
                        self.bug('file {} should be relocated to directory {}/'.format(f, root_dir))
                    else:
                        self.bug('There should be only one directory at the root of the archive')

    def report_problems(self):
        if not self.problems:
            self.good('{file} passed sanity check')
        for value in self.problems:
            print(value.format(file=self.fn))

    def guess_project_name(self):
        self.project_name = None
        for f in self.files:
            if 'RayTracer.cpp' in f:
                self.project_name = 'ray'
                break
            if 'menger.cc' in f:
                self.project_name = 'menger'
                break
            if 'animation_loader_saver.cc' in f:
                self.project_name = 'animation'
                break
        if self.project_name is None:
            for f in self.files:
                if 'pmd_reader_impl.inl' in f:
                    self.project_name = 'skinning'
                    break
        if self.project_name is None:
            self.bug('{file} is probably not archiving a course project')

def sancheck(fn, args):
    print(_msg('', '==========SANITY CHECKING {}=========='.format(fn), Fore.WHITE))
    ac = ArchiveChecker(fn)
    ac.collect_problems()
    ac.report_problems()
    print(_msg('', '----------END OF CHECKING {}----------'.format(fn), Fore.WHITE))

if __name__ == '__main__':
    colorama.init()
    parser = argparse.ArgumentParser(description='Sanity check on your archive for submission.',
            formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('files', metavar='FILE', nargs='+',
            help='Archives to check')
    args = parser.parse_args()
    for f in args.files:
        sancheck(f, args)
