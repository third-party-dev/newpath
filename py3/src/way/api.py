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

class WayMode(object):
    MEMORY = 1
    STREAM = 0

    def __init__(self, input=MEMORY, output=STREAM):
        self.input = input
        self.output = output

class WayCtx(object):
    def __init__(self,
            mode=WayMode(),
            out_fd=sys.stdout.fileno(),
            dst=None,
            idx=0,
            npath=None,
            i=0,
            count=0,
            inserted=0,
            dst_off=0):
        self.mode = mode
        self.out_fd = out_fd
        self.dst = dst
        self.idx = idx
        self.npath = npath
        
        self.i = i
        self.count = count
        self.inserted = inserted
        self.dst_off = dst_off


def way_count(path):
    count = 0

    if len(path) == 0:
        return 0
    
    for b in path:
        if b == ':':
            count += 1
    
    if count > 0:
        return count + 1
    
    return 1

def _insert_elem(mode, dst, path, idx, npath):
    count = 0

    if idx == 0:
        # TODO: Implement mode 1
        if mode == 2:
            sys.stdout.write(npath)
            if len(path) > 0:
                sys.stdout.write(':')
    
    i = 0
    for c in path:

        # TODO: Implement mode 1
        if mode == 2:
            sys.stdout.write(c)
        if c == ':':
            count += 1
            if count == idx:
                # TODO: Implement mode 1
                if mode == 2:
                    sys.stdout.write(npath)
                    if len(path) - i > 0:
                        sys.stdout.write(':')
        i += 1
    
    if count + 1 == idx:
        # TODO: Implement mode 1
        if mode == 2:
            if len(path) > 0:
                sys.stdout.write(':')
            sys.stdout.write(npath)
    
def way_insert_print(path, idx, npath):
    _insert_elem(2, None, path, idx, npath)
            

WAY_SEPARATOR = ':'
WAY_NEWLINE = '\n'


def write_char(ctx, c):
    write_cstr(ctx, c)


def write_cstr(ctx, s):
    if s is None:
        return
    if ctx.mode.output == WayMode.STREAM:
        # TODO: Is this the same as _writeall?
        ctx.out_fd.write(s)
    if ctx.mode.output == WayMode.MEMORY:
        print("Memory writes not currently implemented")


def way_iterate_fd(fd, ctx, f):
    while True:
        try:
            s = fd.read(16)
            if len(s) == 0:
                f(None, True, ctx)
                return
            for c in s:
               f(c, False, ctx)
        except EOFError:
            print("EOF\n")
            f(None, True, ctx)
            return
        except Exception as e:
            print("EOF: %s\n" % e)
            sys.exit(1)
        


def way_iterate_mem(s, ctx, f):
    for c in s:
        f(c, False, ctx)
    f(None, True, ctx)


def way_insert_iter(c, eof, ctx):
    if not eof and ctx.inserted == 1:
        write_char(ctx, WAY_SEPARATOR)
        ctx.inserted = 2
    
    write_char(ctx, c)

    if c == WAY_SEPARATOR:
        ctx.count += 1
        if ctx.count == ctx.idx and ctx.inserted == 0:
            write_cstr(ctx, ctx.npath)
            ctx.inserted = 1
    
    #print("eof: %s count: %d idx %d" % (eof, ctx.count, ctx.idx))
    if eof and ctx.count + 1 == ctx.idx:
        if ctx.i > 0:
            write_char(ctx, WAY_SEPARATOR)
        write_cstr(ctx, ctx.npath)
        ctx.inserted = 1
    
    ctx.i += 1


def _insert(mode, out_fd, in_fd, dst, path, idx, npath):
    ctx = WayCtx(mode=mode, out_fd=out_fd, dst=dst, idx=idx, npath=npath)

    if ctx.idx == 0:
        write_cstr(ctx, npath)
        ctx.inserted = 1
    
    if mode.input == WayMode.STREAM:
        way_iterate_fd(in_fd, ctx, way_insert_iter)
    if mode.input == WayMode.MEMORY:
        way_iterate_mem(path, ctx, way_insert_iter)
    
    if ctx.idx < 0:
        if ctx.i > 0:
            write_char(ctx, WAY_SEPARATOR)
        write_cstr(ctx, npath)
    
    sys.exit(0)



def way_insert_mem2fd(out_fd, path, idx, npath):
    mode = WayMode(input=WayMode.MEMORY, output=WayMode.STREAM)
    _insert(mode, out_fd, None, None, path, idx, npath)

def way_insert_fd2fd(out_fd, in_fd, idx, npath):
    mode = WayMode(input=WayMode.STREAM, output=WayMode.STREAM)
    _insert(mode, out_fd, in_fd, None, None, idx, npath)



