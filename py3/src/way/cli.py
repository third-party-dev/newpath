#!/usr/bin/python3

'''
    Copyright (c) 2022 Vincent Agriesti

    Permission is hereby granted, free of charge, to any person obtaining
    a copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the Software
    is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
    IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
    CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
    OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
'''

# Python 3.10 - Latest
#   UNTESTED
# Python 3.7 - Python UTF-8 Mode Added
#   UNTESTED
# Python 3.3 - UCS4 Default Encoding  
#   UNTESTED
# Python 3.2 - UCS2 Default Encoding
#   UNTESTED
# Python 2.7 - Before the big Python 3 Break
#   UNTESTED
# Python 2.6 - Before the Python 2.7 LTS
#   UNTESTED
# Python 2.4 - A big break from Python 2.4
#   UNTESTED
# Python 2.3 - Before the Python 2.4 Break
#   UNTESTED

# http://unix.stackexchange.com/questions/87745/what-does-lc-all-c-do

# libunistring
# China: GB18030, GB2312
# iso-8859-15, iso-8859-2, koi8-r, euc-jp.
# no utf-16 or utf-32



# The Python UTF-8 Mode is enabled if the LC_CTYPE locale is C or POSIX at Python startup (see the PyConfig_Read() function).
# It can be enabled or disabled using the -X utf8 command line option and the PYTHONUTF8 environment variable.

# https://docs.python.org/3/library/codecs.html
# sys.flags.utf8_mode.
# export PYTHONIOENCODING=utf8
#utf-16-be
# Python 3 did changed the system encoding to default to utf-8 (when LC_CTYPE is unicode-aware),
# reload(sys)  # Reload is a hack
# sys.setdefaultencoding('UTF8')

# encodings/cp1252.py

# /etc/default/locale
# LANG=en_US.UTF-8
# LANGUAGE=en_US.UTF-8
# LC_ALL=en_US.UTF-8

# utf_8
# utf-8
# utf8
# latin1
# iso8859-1
# mbcs (Windows only)
# ascii
# us-ascii
# utf-16
# utf16
# utf-32
# utf32
# latin-1
# iso-8859-1

#legacy_windows_fs_encoding
# nl_langinfo(CODESET)

import os
import sys

from way.api import *
           

def is_decimal(arg):
    # TODO: Check that we're NOT octal.
    for b in range(arg):
        if int(b) < 0x30 or c > 0x39:
            return False

    return True


def is_hex(arg):
    if len(arg) < 3 or int(arg[0]) != 0x30 or (int(arg[1]) != 0x78 and int(arg[1] != 0x58)):
        return False
    
    for _c in arg[2:]:
        c = int(_c)
        if (c < 0x30 or c > 0x39) and (c < 0x61 and c > 0x66) and (c < 0x41 and c > 0x46):
            return False
    
    return True


def is_octal(arg):
    if len(arg) < 2 or int(arg[0]) != 0x30:
        return False
    
    for _c in arg[1:]:
        c = int(_c)
        if c < 0x30 or c > 0x37:
            return False

    return True


def is_valid_index(arg):
    return is_decimal(arg) or is_hex(arg) or is_octal(arg)


def try_convert(arg):
    try:
        if len(arg) >= 1 and arg[0] == 0:
            if len(arg) >= 3 and (int(arg[1]) == 0x58 or int(arg[1]) == 0x78):
                return int(arg, 16)
            return int(arg, 8)
        return int(arg, 10)
    except:
        return None

def insert_usage():
    print("Insert Usage: ...")


def process_insert(command, path, args, mode):
    idx = 0
    index_given = False

    i = 0
    while i < len(args):
        if args[i] == "--":
            i += 1
            break
        elif args[i][0] == '-':
            if args[i] == "--help":
                insert_usage()
                sys.exit(0)
            elif (args[i] == "-i" or args[i] == "--index") and not index_given:
                index_given = True
                if i + 1 < len(args):
                    i += 1
                    idx = try_convert(args[i])
                    if idx == None:
                        insert_usage()
                        sys.exit(1)
            elif (args[i] == "-t" or args[i] == "--tail") and not index_given:
                index_given = True
                if mode.input == WayMode.MEMORY:
                    idx = way_count(path)
                elif mode.input == WayMode.STREAM:
                    idx = -1
            else:
                insert_usage()
                sys.exit(1)
            i += 1
            continue
        break
    
    if len(args) - i == 1:
        npath = args[i]
        if mode.input == WayMode.MEMORY:
            way_insert_mem2fd(sys.stdout, path, idx, npath)
        if mode.input == WayMode.STREAM:
            way_insert_fd2fd(sys.stdout, sys.stdin, idx, npath)
        sys.exit(0)
    
    insert_usage()
    sys.exit(1)


def main_usage():
    print("Usage: %s ..." % sys.argv[0])



import locale
def main():

    # print("sys.getdefaultencoding: %s" % sys.getdefaultencoding())
    # print("locale.getpreferredencoding: %s" % locale.getpreferredencoding())
    # print("sys.stdin.encoding: %s" % sys.stdin.encoding)
    # print("sys.stdout.encoding: %s" % sys.stdout.encoding)
    # print("sys.getfilesystemencoding: %s" % sys.getfilesystemencoding())
    # # PY3 Only: print("sys.flags.utf8_mode: %s" % sys.flags.utf8_mode)
    # # PY3 Only: print("os.device_encoding: %s" % os.device_encoding(0))
    
    mode = WayMode(input=WayMode.MEMORY, output=WayMode.STREAM)
    subcommand = None
    path = None

    command = sys.argv[0]

    i = 1
    for p in sys.argv[1:]:
        if p[0] == '-':
            if p == "--help":
                main_usage()
                sys.exit(1)
            else:
                main_usage()
                sys.exit(1)
        elif subcommand == None and p[0] != '-':
            subcommand = p
            i += 1
            break
        i += 1

    if not os.isatty(sys.stdin.fileno()):
        mode.input = WayMode.STREAM

    if (mode.input == WayMode.MEMORY):
        path = os.getenv("PATH")

    if len(sys.argv) <= 1:
        if mode.input == WayMode.MEMORY:
            print(path)
            sys.exit(0)
        else:
            while True:
                try:
                    print(sys.stdin.read())
                except EOFError:
                    sys.exit(0)
                except:
                    sys.exit(1)
    
    if subcommand == "insert":
        process_insert(sys.argv[0], path, sys.argv[i:], mode)
    
    print(path)
    main_usage()
    sys.exit(1)


if __name__ == "__main__":
    main()