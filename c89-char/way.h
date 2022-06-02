#ifndef WAY_H
#define WAY_H


/* Data structure that directs the IO mode of the way api. */
#define WAY_MEMORY (1)
#define WAY_STREAM (0)
struct way_mode {
    unsigned int input : 1;
    unsigned int output : 1;
};


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


#endif /* WAY_H */