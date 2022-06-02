#ifndef WAY_H
#define WAY_H

#define WAY_MEMORY (1)
#define WAY_STREAM (0)

struct way_mode {
    unsigned int input : 1;
    unsigned int output : 1;
};

size_t way_count_chars(const char *path, const size_t path_len);

size_t way_count_elems(const char *path, const size_t path_len);

void way_insert_set(char *dst, size_t *dst_len, char *path, size_t path_len, int idx, char *npath, size_t npath_len);

void way_insert_print(char *path, size_t path_len, int idx, char *npath);

void way_delete_set(char *dst, size_t *dst_len, char *path, size_t path_len, int idx);

void way_delete_print(char *path, size_t path_len, int idx);

void way_get_print(char *path, size_t path_len, int idx);

#endif /* WAY_H */