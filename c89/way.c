#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

size_t count_elems(const char *path, const size_t path_len) {
  size_t count = 0;
  int i;
  
  for (i = 0; i < path_len; ++i) {
    if (path[i] == ':') {
      count += 1;
    }
  }
  
  return count + 1;
}

#define STR_EQ(a, b) (strcmp(a, b) == 0)

void _insert_elem(
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

void _delete_elem(
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

void _get_elem(
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

/* ================= CLI Handling =================== */

void print_usage(const char *command)
{
    printf("Usage: %s [options] <subcommand> [options] <args>\n", command);
}

#define IS_DECIMAL(c) (c >= '0' && c <= '9')
int is_decimal(const char *str) {
    int i = 0;
    for (i = 0; i < strlen(str); ++i) {
        if (!IS_DECIMAL(str[i])) return 0;
    }
    return 1;
}

#define IS_HEX(c) ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))
int is_hex(const char *str) {
    int i;

    if (strlen(str) < 3) return 0;
    if (str[0] != '0') return 0;
    if (str[1] != 'x' && str[1] != 'X') return 0;

    for (i = 2; i < strlen(str); ++i) {
        if (!IS_HEX(str[i])) return 0;
    }

    return 1;
}

#define IS_OCTAL(c) (c >= '0' && c <= '7')
int is_octal(const char *str) {
    int i;

    if (strlen(str) < 2) return 0;
    if (str[0] != '0') return 0;
    
    for (i = 1; i < strlen(str); ++i) {
        if (!IS_HEX(str[i])) return 0;
    }
    
    return 1;
}

void insert_usage(char *command)
{
    printf("Usage: %s [options] insert [options] <args>\n", command);
}

void process_insert_argv(char *command, char *path, size_t path_len, int argc, char **argv)
{
    int idx = 0;
    char *tgtpath = NULL;

    int index_used = 0;
    int tail_used = 0;

    int i;
    for (i = 0; i < argc; ++i) {
        if (strcmp(argv[i], "--") == 0) {
            ++i;
            break;
        }
        else if (*argv[i] == '-')
        {
            if (strcmp(argv[i],"--help") == 0) {
                goto usage;
            }
            else if (STR_EQ(argv[i], "--index") || STR_EQ(argv[i], "-i") && !tail_used) {  
                index_used = 1;
                if (i + 1 < argc) {
                    ++i;
                    if (is_decimal(argv[i]) || is_hex(argv[i]) || is_octal(argv[i])) {
                        idx = strtol(argv[i], NULL, 0);
                        if (idx == LONG_MIN || idx == LONG_MAX || errno == ERANGE) {
                            goto usage;
                        }
                    } else {
                        goto usage;
                    }
                }
            }
            else if (STR_EQ(argv[i], "--tail") || STR_EQ(argv[i], "-t") && !index_used) {
                tail_used = 1;
                idx = count_elems(path, path_len);
            }
            /* !Other subcommand options go here. */
            else {
                goto usage;
            }
            continue;
        }
        break;
    }

    /* Process subcommand arguments */
    if (argc - i == 1) {
        tgtpath = argv[i];
        way_insert_print(path, path_len, idx, tgtpath);
        exit(0);
    }

usage:
    insert_usage(command);
    exit(1);
}

void delete_usage(char *command)
{
    printf("Usage: %s [options] delete [options] <args>\n", command);
}

void process_delete_argv(char *command, char *path, size_t path_len, int argc, char **argv)
{
    int idx = 0;

    int index_used = 0;
    int tail_used = 0;

    int i;
    for (i = 0; i < argc; ++i) {
        if (strcmp(argv[i], "--") == 0) {
            ++i;
            break;
        }
        else if (*argv[i] == '-')
        {
            if (strcmp(argv[i],"--help") == 0) {
                goto usage;
            }
            else if (STR_EQ(argv[i], "--index") || STR_EQ(argv[i], "-i") && !tail_used) {
                index_used = 1;  
                if (i + 1 < argc) {
                    ++i;
                    /* TODO: Need _better_ error check */
                    if (is_decimal(argv[i]) || is_hex(argv[i]) || is_octal(argv[i])) {
                        idx = strtol(argv[i], NULL, 0);
                        if (idx == LONG_MIN || idx == LONG_MAX || errno == ERANGE) {
                            goto usage;
                        }
                    } else {
                        goto usage;
                    }
                }
            }
            else if (STR_EQ(argv[i], "--tail") || STR_EQ(argv[i], "-t") && !index_used) {
                tail_used = 1;
                idx = count_elems(path, path_len) - 1;
            }
            /* !Other subcommand options go here. */
            else {
                goto usage;
            }
            continue;
        }
        break;
    }

    if (argc - i > 0) {
        goto usage;
    }

    way_delete_print(path, path_len, idx);
    exit(0);

usage:
    delete_usage(command);
    exit(1);
}

void count_usage(const char *command)
{
    printf("Usage: %s [options] count [options] <args>\n", command);
}

void process_count_argv(
    const char *command, char *path, size_t path_len, int argc, char **argv)
{
    int i;
    for (i = 0; i < argc; ++i) {
        if (strcmp(argv[i], "--") == 0) {
            ++i;
            break;
        }
        else if (*argv[i] == '-')
        {
            if (strcmp(argv[i],"--help")) {
                goto usage;
            }
            /* !Other subcommand options go here. */
            else {
                goto usage;
            }
            continue;
        }
        break;
    }

    if (argc - i > 0) {
        goto usage;
    }

    printf("%u\n", (unsigned int)count_elems(path, path_len));
    exit(0);

usage:
    count_usage(command);
    exit(1);
}

void len_usage(const char *command)
{
    printf("Usage: %s [options] len [options] <args>\n", command);
}

void process_len_argv(
    const char *command, 
    const char *path, 
    const size_t path_len, 
    int argc, 
    char **argv)
{
    int i;
    for (i = 0; i < argc; ++i) {
        if (strcmp(argv[i], "--") == 0) {
            ++i;
            break;
        }
        else if (*argv[i] == '-')
        {
            if (strcmp(argv[i],"--help")) {
                goto usage;
            }
            /* !Other subcommand options go here. */
            else {
                goto usage;
            }
            continue;
        }
        break;
    }

    if (argc - i > 0) {
        goto usage;
    }

    printf("%u", (unsigned int)path_len);
    exit(0);

usage:
    len_usage(command);
    exit(1);
}

void get_usage(char *command)
{
    printf("Usage: %s [options] get [options] <args>\n", command);
}

void process_get_argv(char *command, char *path, size_t path_len, int argc, char **argv)
{
    int idx = 0;
    char *sub;
    size_t sub_len;
    char *res;
    int i;
    int tail_used = 0;
    int index_used = 0;

    if (path == NULL || path_len == 0) {
        exit(0);
    }

    for (i = 0; i < argc; ++i) {
        if (strcmp(argv[i], "--") == 0) {
            ++i;
            break;
        }
        else if (*argv[i] == '-')
        {
            if (strcmp(argv[i],"--help") == 0) {
                goto usage;
            }
            else if (STR_EQ(argv[i], "--index") || STR_EQ(argv[i], "-i") && !tail_used) {  
                index_used = 1;
                if (i + 1 < argc) {
                    ++i;
                    if (is_decimal(argv[i]) || is_hex(argv[i]) || is_octal(argv[i])) {
                        idx = strtol(argv[i], NULL, 0);
                        if (idx == LONG_MIN || idx == LONG_MAX || errno == ERANGE) {
                            goto usage;
                        }
                    } else {
                        goto usage;
                    }
                }
            }
            else if (STR_EQ(argv[i], "--tail") || STR_EQ(argv[i], "-t") && !index_used) { 
                tail_used = 1; 
                idx = count_elems(path, path_len) - 1;
            }
            /* !Other subcommand options go here. */
            else {
                goto usage;
            }
            continue;
        }
        break;
    }

    if (argc - i > 0) {
        goto usage;
    }

    /* Process subcommand arguments */
    way_get_print(path, path_len, idx);
    exit(0);

usage:
    get_usage(command);
    exit(1);
}

int main(int argc, char **argv)
{
    int i;
    
    int have_subcommand = 0;
    char *subcommand = NULL;

    char *envpath = getenv("PATH");
    size_t envpath_len = strlen(envpath);

    /*
      Usage: <command> [options] <sub command> [options] <args>
      States:
       - Command
       - Command Options
       - Subcommand
       - Subcommand Options
       - Subcommand Arguments
    */

    /* 1. Get command */
    char *command = argv[0];

    /* 2. Process options until sub_command */
    if (argc == 1)
    {
        printf("%s", envpath);
        exit(0);
    }

    for (i = 1; i < argc; ++i) {
        if (*argv[i] == '-')
        {
            if (STR_EQ(argv[i], "--help")) {
                goto usage;
            }
            /* !Other subcommand options go here. */
            else {
                goto usage;
            }
            continue;
        }
        else if (subcommand == NULL && *argv[i] != '-') {
            subcommand = argv[i];
            ++i;
            break;
        }
    }

    if (subcommand != NULL) {
        if (STR_EQ(subcommand, "insert")) {
            process_insert_argv(command, envpath, envpath_len, argc - i, argv + i);
        }
        if (STR_EQ(subcommand, "delete")) {
            process_delete_argv(command, envpath, envpath_len, argc - i, argv + i);
        }
        if (STR_EQ(subcommand, "count")) {
            process_count_argv(command, envpath, envpath_len, argc - i, argv + i);
        }
        if (STR_EQ(subcommand, "len")) {
            process_len_argv(command, envpath, envpath_len, argc - i, argv + i);
        }
        if (STR_EQ(subcommand, "get")) {
            process_get_argv(command, envpath, envpath_len, argc - i, argv + i);
        }
    }

    /* TODO: Consider path output options? */

usage:
    print_usage(command);
    return 1;
}