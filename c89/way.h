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

#ifndef WAY_H
#define WAY_H


/* Data structure that directs the IO mode of the way api. */
#define WAY_MEMORY (1)
#define WAY_STREAM (0)
struct way_mode {
    unsigned int input : 1;
    unsigned int output : 1;
};

#ifdef _WIN32
#define WAY_SEPARATOR ';'
#else
#define WAY_SEPARATOR ':'
#endif

#define WAY_NEWLINE '\n'

/* Counts the total multi-byte characters in memory range. */
size_t way_chars_mem(
    const char *path,
    const size_t path_len);


/* Counts elements in PATH in given stream. */
size_t way_count_fd(
    int fd);


/* Counts elements in PATH in given memory stream. */
size_t way_count_mem(
    char *s,
    size_t s_len);


/* Inserts PATH element from given memory range to stream. */
void way_insert_mem2fd(
    int out_fd,
    char *path,
    size_t path_len,
    int idx,
    char *npath,
    size_t npath_len);


/* Inserts PATH element from given stream to stream. */
void way_insert_fd2fd(
    int out_fd,
    int in_fd,
    int idx,
    char *npath,
    size_t npath_len);


/* Replaces PATH element from given memory range to stream. */
void way_replace_mem2fd(
    int out_fd,
    char *path,
    size_t path_len,
    int idx,
    char *npath,
    size_t npath_len);


/* Replaces PATH element from given stream to stream. */
void way_replace_fd2fd(
    int out_fd,
    int in_fd,
    int idx,
    char *npath,
    size_t npath_len);


/* Remove PATH element from given memory range to stream. */
void way_delete_mem2fd(
    int out_fd,
    char *path,
    size_t path_len,
    int idx);


/* Remove PATH element from given stream to stream. */
void way_delete_fd2fd(
    int out_fd,
    int in_fd,
    int idx);


/* Get PATH element from given memory range to stream. */
void way_get_mem2fd(
    int out_fd,
    char *path,
    size_t path_len,
    int idx);


/* Get PATH element from given stream to stream. */
void way_get_fd2fd(
    int out_fd,
    int in_fd,
    int idx);


/* List elements on lines from given memory range to stream. */
void way_list_mem2fd(
    int out_fd,
    char *path,
    size_t path_len);


/* List elements on lines from given stream to stream. */
void way_list_fd2fd(
    int out_fd,
    int in_fd);


/* Join whitespace delimited elements from given memory range to stream. */
void way_join_mem2fd(
    int out_fd,
    char *path,
    size_t path_len);


/* Join whitespace delimited elements from given stream to stream. */
void way_join_fd2fd(
    int out_fd,
    int in_fd);


#endif /* WAY_H */