#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "way.h"

/* --- Start of unistd.h definitions --- */

typedef long int ssize_t;

/* --- End of unistd.h definitions --- */

ssize_t _writeall(int fd, const void *buf, size_t count)
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

size_t way_count_chars(
    const char *path,
    const size_t path_len) {
    return strlen_mb(path, path_len);
}


int _way_putchar(int c, int eof, void * _) {
    (void)_;
    putchar(c);
}


int _way_count_iter(int c, int eof, void *count) {
    if ((char)c == ':') (*(size_t *)count)++;
}

int _way_set_iter(int c, int eof, void *dst) {
    (*(char *)dst) = (char)c;
}


void _way_iterate_fd(int fd, void *data, int(*f)(int,int,void*)) {

    char buffer[sizeof(size_t)];
    ssize_t bytes;
    int i;
    while ((bytes = read(fd, buffer, sizeof(size_t)))) {
        if (bytes == 0) { f(buffer[i], 1, data); exit(0); }
        if (bytes < 0) exit(1);
        if (f) for (i = 0; i < bytes; ++i) f(buffer[i], 0, data);
    }

}


void _way_iterate_cstr(char *s, size_t s_len, void *data, int(*f)(int,int,void*)) {
    int i;
    for (i = 0; i < s_len; ++i) {
        f(s[i], 0, data);
    }
    f(s[i], 1, data);
}


size_t way_count_fd(int fd) {

  size_t count = 0;
  
  _way_iterate_fd(fd, (void *)&count, _way_count_iter);

  return count > 0 ? count + 1 : 1;
}


size_t way_count_cstr(char *s, size_t s_len) {

  size_t count = 0;
  
  _way_iterate_cstr(s, s_len, (void *)&count, _way_count_iter);

  return count > 0 ? count + 1 : 1;
}

size_t way_count_elems(
    const char *path,
    const size_t path_len) {
  size_t count = 0;
  int i;
  
  if (path_len == 0) return 0;

  for (i = 0; i < path_len; ++i) {
    if (path[i] == ':') {
      count += 1;
    }
  }

  return count > 0 ? count + 1 : 1;
}


struct _insert_ctx {
    int idx;
    char *npath;
    size_t npath_len;
    struct way_mode mode;
    int out_fd;

    int i;
    size_t count;
    int inserted; /* 1 - inserted, 2 - added ':' */
};


int _way_insert_iter(int cparam, int eof, void *data)
{
    struct _insert_ctx *ctx = (struct _insert_ctx *)data;
    char c = (char)cparam;

    if (!eof && ctx->inserted == 1) {
        if (ctx->mode.output == WAY_STREAM)
            (void)_writeall(ctx->out_fd, ":", 1);
        ctx->inserted = 2;
    }

    if (ctx->mode.output == WAY_STREAM)
        (void)_writeall(ctx->out_fd, &c, 1);

    if (c == ':') {
        ctx->count++;
        if (ctx->count == ctx->idx && ctx->inserted == 0) {
            if (ctx->mode.output == WAY_STREAM) {
                _writeall(ctx->out_fd, ctx->npath, ctx->npath_len);
            }
            ctx->inserted = 1;
        }
    }

    if (eof && ctx->count + 1 == ctx->idx) {
        if (ctx->mode.output == WAY_STREAM) {
            if (ctx->i > 0) _writeall(ctx->out_fd, ":", 1);
            _writeall(ctx->out_fd, ctx->npath, ctx->npath_len);
        }
        ctx->inserted = 1;
    }

    ctx->i++;
}


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
    
    struct _insert_ctx ctx = { 0 };
    ctx.idx = idx;
    ctx.npath = npath;
    ctx.npath_len = npath_len;
    ctx.mode = mode;
    ctx.out_fd = out_fd;

    if (ctx.idx == 0) {
        if (mode.output == WAY_STREAM) {
            _writeall(out_fd, npath, npath_len);
        }
        ctx.inserted = 1;
    }

    if (mode.input == WAY_STREAM)
        _way_iterate_fd(in_fd, (void *)&ctx, _way_insert_iter);
    if (mode.input == WAY_MEMORY)
        _way_iterate_cstr(path, path_len, (void *)&ctx, _way_insert_iter);

    if (ctx.idx < 0) { /* tail */
        if (mode.output == WAY_STREAM) {
            if (ctx.i > 0) _writeall(out_fd, ":", 1);
            _writeall(out_fd, npath, npath_len);
        }
    }
}


#ifndef USE_INSERT_ELEM
static void _insert_elem(
    int mode,
    char *dst,
    size_t *dst_len,
    char *path, 
    size_t path_len, 
    int idx, 
    char *npath, 
    size_t npath_len) {

    size_t count = 0;
    size_t dst_idx = 0;
    int i;
    
    /* //! Not sure if making noise about multi-inserts is correct. */
    /* // TODO: If checking this, should check for premature NULL. */
    /*
    size_t elem_count = 0;
    elem_count = way_count_elems(npath, npath_len);
    if (elem_count > 1) {
        #define MULTI_INSERT_WARNING \
          "WARNING: Inserting %u items. ':' not allowed in PATH.\n"
        fprintf(stderr, MULTI_INSERT_WARNING, (unsigned int)elem_count);
    }
    */

    if (idx == 0) {
        if (mode == 1) {
            if (dst) {
                memcpy(&dst[dst_idx], npath, npath_len);
                dst_idx += npath_len;
            }
            if (dst_len) *dst_len += npath_len;
            if (path_len > 0) dst[dst_idx++] = ':';
        }
        if (mode == 2) {
            printf("%s", npath);
            if (path_len > 0) putchar(':');
        }
    }

    for (i = 0; i < path_len; ++i) {
        if (mode == 1) {
            if (dst) dst[dst_idx++] = path[i];
            if (dst_len) *dst_len = dst_idx;
        }
        if (mode == 2) {
            putchar(path[i]);
        }
        if (path[i] == ':') {
            ++count;
            if (count == idx) {
                if (mode == 1) {
                    if (dst) {
                        memcpy(&dst[dst_idx], npath, npath_len);
                        dst_idx += npath_len;
                    }
                    if (dst_len) *dst_len += npath_len;
                    if (path_len - i > 0) dst[dst_idx++] = ':';
                }
                if (mode == 2) {
                    printf("%s", npath);
                    if (path_len - i > 0) putchar(':');
                }
            }
        }
    }

    if (count + 1 == idx) {
        if (mode == 1) {
            if (path_len > 0) dst[dst_idx++] = ':';
            if (dst) {
                memcpy(&dst[dst_idx], npath, npath_len);
                dst_idx += npath_len;
            }
            if (dst_len) *dst_len += npath_len;
        }
        if (mode == 2) {
            if (path_len > 0) putchar(':');
            printf("%s", npath);
        }
    }
}
#endif


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
    /* //TODO: Implement this. */
    _insert(mode, -1, in_fd, dst, dst_len, NULL, 0, idx, npath, strlen(npath));
}


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


/* LEGACY */
void way_insert_set(
    char *dst,
    size_t *dst_len,
    char *path,
    size_t path_len,
    int idx,
    char *npath,
    size_t npath_len) {
    _insert_elem(1, dst, dst_len, path, path_len, idx, npath, npath_len);
}


/* LEGACY */
void way_insert_print(
    char *path,
    size_t path_len,
    int idx,
    char *npath) {
    
}

#ifndef USE_DELETE_ELEM
/* LEGACY */
static void _delete_elem(
    int mode, 
    char *dst, 
    size_t *dst_len, 
    char *path, 
    size_t path_len, 
    int idx) {

    size_t count = 0;
    int i;
    size_t dst_idx = 0;

    for (i = 0; i < path_len; ++i) {
        if (path[i] == ':') {
            ++count;
            if (idx == 0 && count == 1) continue;
            if (idx == count) continue;
        }
        if (idx != count) {
            if (mode == 1) {
                if (dst) dst[dst_idx++] = path[i];
                if (dst_len) *dst_len++;
            }
            if (mode == 2) putchar(path[i]);
        }
    }
}
#endif


int _way_delete_iter(int cparam, int eof, void *data)
{
    struct _insert_ctx *ctx = (struct _insert_ctx *)data;
    char c = (char)cparam;

    if (c == ':') {
        ctx->count++;
        if (ctx->idx == 0 && ctx->count == 1) goto next;
        if (ctx->idx == ctx->count) goto next;
    }
    if (ctx->idx != ctx->count) {
        _way_putchar(cparam, 0, NULL);
    }

next:
    ctx->i++;
}


static void _delete_stream(
    int mode, 
    char *dst, 
    size_t *dst_len, 
    char *path, 
    size_t path_len, 
    int idx) {

    size_t count = 0;
    int i;
    size_t dst_idx = 0;

    struct _insert_ctx ctx = { 0 };


    /*for (i = 0; i < path_len; ++i) {
        if (path[i] == ':') {
            ++count;
            if (idx == 0 && count == 1) continue;
            if (idx == count) continue;
        }
        if (idx != count) {
            if (mode == 1) {
                if (dst) dst[dst_idx++] = path[i];
                if (dst_len) *dst_len++;
            }
            if (mode == 2) putchar(path[i]);
        }
    }*/
    _way_iterate_fd(fileno(stdin), (void *)&ctx, _way_delete_iter);
}


/* LEGACY */
void way_delete_set(
    char *dst,
    size_t *dst_len,
    char *path,
    size_t path_len,
    int idx) {
    _delete_elem(1, dst, dst_len, path, path_len, idx);
}


/* LEGACY */
void way_delete_print(
    char *path,
    size_t path_len,
    int idx) {
    _delete_stream(2, NULL, NULL, path, path_len, idx);
}


#ifndef USE_GET_ELEM
/* LEGACY */
static void _get_elem(
    int mode, 
    char *dst, 
    size_t *dst_len, 
    char *path, 
    size_t path_len, 
    int idx) {

    size_t count = 0;
    int i;
    size_t dst_idx = 0;

    for (i = 0; i < path_len; ++i) {
        if (path[i] == ':') ++count;
        if (path[i] == ':' && idx == count) continue;
        if (idx == count) {
            if (mode == 1) {
                if (dst) dst[dst_idx++] = path[i];
                if (dst_len) *dst_len++;
            }
            if (mode == 2) {
                putchar(path[i]);
            }
        }
    }
}
#endif


int _way_get_iter(int cparam, int eof, void *data)
{
    struct _insert_ctx *ctx = (struct _insert_ctx *)data;
    char c = (char)cparam;

    if (c == ':') ctx->count++;
    if (c == ':' && ctx->idx == ctx->count) goto next;
    if (ctx->idx == ctx->count) {
        _way_putchar(cparam, 0, NULL);
    }

next:
    ctx->i++;
}


static void _get_stream(
    int mode, 
    char *dst, 
    size_t *dst_len, 
    char *path, 
    size_t path_len, 
    int idx) {

    size_t count = 0;
    int i;
    size_t dst_idx = 0;

    struct _insert_ctx ctx = { 0 };
    ctx.idx = idx;

    _way_iterate_fd(fileno(stdin), (void *)&ctx, _way_get_iter);

}


/* LEGACY */
void way_get_set(
    char *dst,
    size_t *dst_len,
    char *path,
    size_t path_len,
    int idx) {
    _get_elem(1, dst, dst_len, path, path_len, idx);
}


/* LEGACY */
void way_get_print(
    char *path,
    size_t path_len,
    int idx) {
    _get_stream(2, NULL, NULL, path, path_len, idx);
}

