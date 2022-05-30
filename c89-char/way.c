#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Note: This includes non-visible variation selectors. */
static size_t strlen_mb(const char* path, const size_t path_len)
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

size_t way_count_chars(const char *path, const size_t path_len) {
    return strlen_mb(path, path_len);
}

size_t way_count_elems(const char *path, const size_t path_len) {
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

    /* Get the count. */
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

void way_insert_set(char *dst, size_t *dst_len, char *path, size_t path_len, int idx, char *npath, size_t npath_len) {
    _insert_elem(1, dst, dst_len, path, path_len, idx, npath, npath_len);
}

void way_insert_print(char *path, size_t path_len, int idx, char *npath) {
    _insert_elem(2, NULL, NULL, path, path_len, idx, npath, strlen(npath));
}

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

void way_delete_set(char *dst, size_t *dst_len, char *path, size_t path_len, int idx) {
    _delete_elem(1, dst, dst_len, path, path_len, idx);
}

void way_delete_print(char *path, size_t path_len, int idx) {
    _delete_elem(2, NULL, NULL, path, path_len, idx);
}

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

void way_get_set(char *dst, size_t *dst_len, char *path, size_t path_len, int idx) {
    _get_elem(1, dst, dst_len, path, path_len, idx);
}

void way_get_print(char *path, size_t path_len, int idx) {
    _get_elem(2, NULL, NULL, path, path_len, idx);
}

