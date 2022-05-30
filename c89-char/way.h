#ifndef WAY_H
#define WAY_H

size_t way_count_elems(const char *path, const size_t path_len);

void way_insert_set(char *dst, size_t *dst_len, char *path, size_t path_len, int idx, char *npath, size_t npath_len);

void way_insert_print(char *path, size_t path_len, int idx, char *npath);

void way_delete_set(char *dst, size_t *dst_len, char *path, size_t path_len, int idx);

void way_delete_print(char *path, size_t path_len, int idx);

void way_get_print(char *path, size_t path_len, int idx);

#endif /* WAY_H */