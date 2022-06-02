/*
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
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "way.h"

/* Note: Avoiding inclusion of unistd.h for portability. */
/* --- Start of unistd.h definitions --- */
typedef long int ssize_t;
/* --- End of unistd.h definitions --- */


/* Data structure used to store parameters and state for iterators. */
struct way_iter_ctx {
    /* Parameters */
    struct way_mode mode;
    int out_fd;
    char *dst;
    size_t *dst_len;
    int idx;
    char *npath;
    size_t npath_len;

    /* State */
    int i;
    size_t count;
    int inserted; /* 1 - inserted, 2 - added ':' */
    size_t dst_off;
};


/* Continuously attempt to write() bytes until all bytes written to stream. */
static ssize_t _writeall(int fd, const void *buf, size_t count)
{
    unsigned char *ptr = (unsigned char *)buf;
    size_t offset = 0;
    ssize_t bytes = 0;
    while ((bytes = write(fd, ptr+offset, count-offset))) {
        if (bytes <= 0) return bytes;
        offset += bytes;
    }
    return count;
}


/* (Private) Write out byte of memory dependent on ctx->mode. */
static void write_char(struct way_iter_ctx *ctx, char c) {
    if (ctx->mode.output == WAY_STREAM)
        _writeall(ctx->out_fd, &c, sizeof(c));
    if (ctx->mode.output == WAY_MEMORY) {
        if (ctx->dst && ctx->dst_len && ctx->dst_off < *ctx->dst_len) {
            ctx->dst[ctx->dst_off++] = c;
        }
    }
}


/* (Private) Write out memory range dependent on ctx->mode. */
static void write_cstr(struct way_iter_ctx *ctx, char *s, size_t s_len) {
    if (ctx->mode.output == WAY_STREAM)
        _writeall(ctx->out_fd, s, s_len);
    if (ctx->mode.output == WAY_MEMORY) {
        if (ctx->dst && ctx->dst_len && ctx->dst_off < *ctx->dst_len) {
            memcpy(ctx->dst + ctx->dst_off, s, s_len);
            ctx->dst_off += s_len;
        }
    }
}


/* (Private) Iterate and call handler on each byte in stream. */
static void way_iterate_fd(int fd, void *data, int(*f)(int,int,void*)) {
    char buffer[sizeof(size_t)];
    ssize_t bytes;
    int i;
    while ((bytes = read(fd, buffer, sizeof(size_t)))) {
        if (bytes == 0) { f(buffer[i], 1, data); exit(0); }
        if (bytes < 0) exit(1);
        if (f) for (i = 0; i < bytes; ++i) f(buffer[i], 0, data);
    }
}


/* (Private) Iterate and call handler on each byte in given memory range. */
static void way_iterate_mem(char *s, size_t s_len, void *data, int(*f)(int,int,void*)) {
    int i;
    for (i = 0; i < s_len; ++i) {
        f(s[i], 0, data);
    }
    f(s[i], 1, data);
}


/* (Private) Counts the total multi-byte characters in memory. */
/* Note: This includes non-visible variation selectors. */
static size_t strlen_mb(
    const char* path,
    const size_t path_len)
{
    size_t result = 0;
    char *ptr = (char *)path;
    const char* end = ptr + path_len;
    mblen(NULL, 0); /* reset the conversion state */
    while(ptr < end) {
        int next = mblen(ptr, end - ptr);
        if(next == -1) {
           return (size_t)-1;
           break;
        }
        ptr += next;
        ++result;
    }
    return result;
}


/* Counts the total multi-byte characters in memory range. */
size_t way_chars_mem(
    const char *path,
    const size_t path_len) {
    return strlen_mb(path, path_len);
}


/* //TODO: way_chars_fd */


/* (Private) Iteration handler used to count elements in PATH. */
static int way_count_iter(int c, int eof, void *count) {
    if ((char)c == ':') (*(size_t *)count)++;
}


/* Counts elements in PATH in given stream. */
size_t way_count_fd(int fd) {
  size_t count = 0;
  way_iterate_fd(fd, (void *)&count, way_count_iter);
  return count > 0 ? count + 1 : 1;
}


/* Counts elements in PATH in given memory stream. */
size_t way_count_mem(char *s, size_t s_len) {
  size_t count = 0;
  way_iterate_mem(s, s_len, (void *)&count, way_count_iter);
  return count > 0 ? count + 1 : 1;
}


/* (Private) Iteration handler used to replace a PATH element with user input. */
static int way_replace_iter(int cparam, int eof, void *data)
{
    struct way_iter_ctx *ctx = (struct way_iter_ctx *)data;
    char c = (char)cparam;

    /* Write all the stuff we aren't replacing. */
    if (ctx->idx != ctx->count) {
        if (!eof && ctx->inserted == 1) {
            write_char(ctx, ':');
            ctx->inserted = 2;
        }
        write_char(ctx, c);
    }

    if (c == ':') {
        ctx->count++;
        if (ctx->idx == ctx->count && ctx->inserted == 0) {
            write_cstr(ctx, ctx->npath, ctx->npath_len);
            ctx->inserted = 1;
        }
    }
}


/* (Private) Replaces PATH element with user input. */
static void _replace(
    struct way_mode mode,
    int out_fd,
    int in_fd,
    char *dst,
    size_t *dst_len,
    char *path, 
    size_t path_len, 
    int idx, 
    char *npath, 
    size_t npath_len) {
    
    /* //! Not sure if making noise about multi-inserts is correct. */
    /* // TODO: If checking this, should check for premature NULL. */
    /*
    size_t elem_count = 0;
    elem_count = way_count_mem(npath, npath_len);
    if (elem_count > 1) {
        #define MULTI_INSERT_WARNING \
          "WARNING: Inserting %u items. ':' not allowed in PATH.\n"
        fprintf(stderr, MULTI_INSERT_WARNING, (unsigned int)elem_count);
    }
    */

    struct way_iter_ctx ctx = { 0 };
    ctx.mode = mode;
    ctx.out_fd = out_fd;
    ctx.dst = dst;
    ctx.dst_len = dst_len;
    ctx.idx = idx;
    ctx.npath = npath;
    ctx.npath_len = npath_len;

    if (ctx.idx == 0) {
        write_cstr(&ctx, npath, npath_len);
        ctx.inserted = 1;
    }

    if (mode.input == WAY_STREAM)
        way_iterate_fd(in_fd, (void *)&ctx, way_replace_iter);
    if (mode.input == WAY_MEMORY)
        way_iterate_mem(path, path_len, (void *)&ctx, way_replace_iter);

}


/* Replaces PATH element from given memory range to stream. */
void way_replace_mem2fd(
    int out_fd,
    char *path,
    size_t path_len,
    int idx,
    char *npath,
    size_t npath_len) {
    struct way_mode mode = { 0 };
    mode.input = WAY_MEMORY;
    mode.output = WAY_STREAM;
    _replace(mode, out_fd, -1, NULL, NULL, path, path_len, idx, npath, strlen(npath));
}


/* Replaces PATH element from given stream to stream. */
void way_replace_fd2fd(
    int out_fd,
    int in_fd,
    int idx,
    char *npath,
    size_t npath_len) {
    struct way_mode mode = { 0 };
    mode.input = WAY_STREAM;
    mode.output = WAY_STREAM;
    _replace(mode, out_fd, in_fd, NULL, NULL, NULL, 0, idx, npath, strlen(npath));
}


/* (Private) Iteration handler used to insert a PATH element with user input. */
static int way_insert_iter(int cparam, int eof, void *data)
{
    struct way_iter_ctx *ctx = (struct way_iter_ctx *)data;
    char c = (char)cparam;

    if (!eof && ctx->inserted == 1) {
        write_char(ctx, ':');
        ctx->inserted = 2;
    }

    write_char(ctx, c);

    if (c == ':') {
        ctx->count++;
        if (ctx->count == ctx->idx && ctx->inserted == 0) {
            write_cstr(ctx, ctx->npath, ctx->npath_len);
            ctx->inserted = 1;
        }
    }

    if (eof && ctx->count + 1 == ctx->idx) {
        if (ctx->i > 0) write_char(ctx, ':');
        write_cstr(ctx, ctx->npath, ctx->npath_len);
        ctx->inserted = 1;
    }

    ctx->i++;
}


/* (Private) Inserts PATH element with user input. */
static void _insert(
    struct way_mode mode,
    int out_fd,
    int in_fd,
    char *dst,
    size_t *dst_len,
    char *path, 
    size_t path_len, 
    int idx, 
    char *npath, 
    size_t npath_len) {
    
    /* //! Not sure if making noise about multi-inserts is correct. */
    /* // TODO: If checking this, should check for premature NULL. */
    /*
    size_t elem_count = 0;
    elem_count = way_count_mem(npath, npath_len);
    if (elem_count > 1) {
        #define MULTI_INSERT_WARNING \
          "WARNING: Inserting %u items. ':' not allowed in PATH.\n"
        fprintf(stderr, MULTI_INSERT_WARNING, (unsigned int)elem_count);
    }
    */

    struct way_iter_ctx ctx = { 0 };
    ctx.mode = mode;
    ctx.out_fd = out_fd;
    ctx.dst = dst;
    ctx.dst_len = dst_len;
    ctx.idx = idx;
    ctx.npath = npath;
    ctx.npath_len = npath_len;

    if (ctx.idx == 0) {
        write_cstr(&ctx, npath, npath_len);
        ctx.inserted = 1;
    }

    if (mode.input == WAY_STREAM)
        way_iterate_fd(in_fd, (void *)&ctx, way_insert_iter);
    if (mode.input == WAY_MEMORY)
        way_iterate_mem(path, path_len, (void *)&ctx, way_insert_iter);

    if (ctx.idx < 0) { /* tail */
        if (ctx.i > 0) write_char(&ctx, ':');
        write_cstr(&ctx, npath, npath_len);
    }
}


/* Inserts PATH element form given stream to memory range. */
void way_insert_fd2mem(
    int in_fd,
    char *dst,
    size_t *dst_len,
    int idx,
    char *npath,
    size_t npath_len) {
    struct way_mode mode;
    mode.input = WAY_STREAM;
    mode.output = WAY_MEMORY;
    _insert(mode, -1, in_fd, dst, dst_len, NULL, 0, idx, npath, strlen(npath));
}


/* Inserts PATH element from given memory range to stream. */
void way_insert_mem2fd(
    int out_fd,
    char *path,
    size_t path_len,
    int idx,
    char *npath,
    size_t npath_len) {
    struct way_mode mode = { 0 };
    mode.input = WAY_MEMORY;
    mode.output = WAY_STREAM;
    _insert(mode, out_fd, -1, NULL, NULL, path, path_len, idx, npath, strlen(npath));
}


/* Inserts PATH element from given stream to stream. */
void way_insert_fd2fd(
    int out_fd,
    int in_fd,
    int idx,
    char *npath,
    size_t npath_len) {
    struct way_mode mode = { 0 };
    mode.input = WAY_STREAM;
    mode.output = WAY_STREAM;
    _insert(mode, out_fd, in_fd, NULL, NULL, NULL, 0, idx, npath, strlen(npath));
}


/* (Private) Iteration handler used to remove a PATH element. */
static int way_delete_iter(int cparam, int eof, void *data)
{
    struct way_iter_ctx *ctx = (struct way_iter_ctx *)data;
    char c = (char)cparam;

    if (c == ':') {
        ctx->count++;
        if (ctx->idx == 0 && ctx->count == 1) return;
        if (ctx->idx == ctx->count) return;
    }
    if (ctx->idx != ctx->count) {
        write_char(ctx, c);
    }
}


/* (Private) Remove PATH element. */
static void _delete(
    struct way_mode mode,
    int out_fd,
    int in_fd,
    char *dst,
    size_t *dst_len,
    char *path, 
    size_t path_len, 
    int idx) {

    struct way_iter_ctx ctx = { 0 };
    ctx.mode = mode;
    ctx.idx = idx;
    ctx.out_fd = out_fd;
    ctx.dst = dst;
    ctx.dst_len = dst_len;

    if (mode.input == WAY_STREAM)
        way_iterate_fd(in_fd, (void *)&ctx, way_delete_iter);
    if (mode.input == WAY_MEMORY)
        way_iterate_mem(path, path_len, (void *)&ctx, way_delete_iter);

}


/* Remove PATH element from given memory range to stream. */
void way_delete_mem2fd(
    int out_fd,
    char *path,
    size_t path_len,
    int idx) {
    struct way_mode mode = { 0 };
    mode.input = WAY_MEMORY;
    mode.output = WAY_STREAM;
    _delete(mode, out_fd, -1, NULL, NULL, path, path_len, idx);
}


/* Remove PATH element from given stream to stream. */
void way_delete_fd2fd(
    int out_fd,
    int in_fd,
    int idx) {
    struct way_mode mode = { 0 };
    mode.input = WAY_STREAM;
    mode.output = WAY_STREAM;
    _delete(mode, out_fd, in_fd, NULL, NULL, NULL, 0, idx);
}


/* (Private) Iteration handler used to fetch a PATH element. */
static int way_get_iter(int cparam, int eof, void *data)
{
    struct way_iter_ctx *ctx = (struct way_iter_ctx *)data;
    char c = (char)cparam;

    if (c == ':') ctx->count++;
    if (c == ':' && ctx->idx == ctx->count) goto next;
    if (ctx->idx == ctx->count) {
        write_char(ctx, c);
    }

next:
    ctx->i++;
}


/* Fetch PATH element. */
static void _get(
    struct way_mode mode, 
    int out_fd,
    int in_fd,
    char *dst,
    size_t *dst_len,
    char *path, 
    size_t path_len, 
    int idx) {

    /*size_t count = 0;
    int i;
    size_t dst_idx = 0;*/

    struct way_iter_ctx ctx = { 0 };
    ctx.mode = mode;
    ctx.dst = dst;
    ctx.dst_len = dst_len;
    ctx.idx = idx;

    if (mode.input == WAY_STREAM)
        way_iterate_fd(in_fd, (void *)&ctx, way_get_iter);
    if (mode.input == WAY_MEMORY)
        way_iterate_mem(path, path_len, (void *)&ctx, way_get_iter);

}


/* Get PATH element from given memory range to stream. */
void way_get_mem2fd(
    int out_fd,
    char *path,
    size_t path_len,
    int idx) {
    struct way_mode mode = { 0 };
    mode.input = WAY_MEMORY;
    mode.output = WAY_STREAM;
    _get(mode, out_fd, -1, NULL, NULL, path, path_len, idx);
}


/* Get PATH element from given stream to stream. */
void way_get_fd2fd(
    int out_fd,
    int in_fd,
    int idx) {
    struct way_mode mode = { 0 };
    mode.input = WAY_STREAM;
    mode.output = WAY_STREAM;
    _get(mode, out_fd, in_fd, NULL, NULL, NULL, 0, idx);
}


