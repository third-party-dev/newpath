#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

#include "way.h"

#define STR_EQ(s1, s2) (strcmp(s1, s2) == 0)

static void print_usage(const char *command)
{
    printf("Usage: %s [options] <subcommand> [options] <args>\n", command);
}

#define IS_DECIMAL(c) (c >= '0' && c <= '9')
static int is_decimal(const char *str) {
    int i = 0;
    for (i = 0; i < strlen(str); ++i) {
        if (!IS_DECIMAL(str[i])) return 0;
    }
    return 1;
}

#define IS_HEX(c) ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))
static int is_hex(const char *str) {
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
static int is_octal(const char *str) {
    int i;

    if (strlen(str) < 2) return 0;
    if (str[0] != '0') return 0;
    
    for (i = 1; i < strlen(str); ++i) {
        if (!IS_HEX(str[i])) return 0;
    }
    
    return 1;
}

static void insert_usage(char *command)
{
    printf("Usage: %s [options] insert [options] <args>\n", command);
}

static void process_insert_argv(char *command, char *path, size_t path_len, int argc, char **argv)
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
                idx = way_count_elems(path, path_len);
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

static void delete_usage(char *command)
{
    printf("Usage: %s [options] delete [options] <args>\n", command);
}

static void process_delete_argv(char *command, char *path, size_t path_len, int argc, char **argv)
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
                idx = way_count_elems(path, path_len) - 1;
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

static void count_usage(const char *command)
{
    printf("Usage: %s [options] count [options] <args>\n", command);
}

static void process_count_argv(
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

    printf("%u\n", (unsigned int)way_count_elems(path, path_len));
    exit(0);

usage:
    count_usage(command);
    exit(1);
}

static void len_usage(const char *command)
{
    printf("Usage: %s [options] len [options] <args>\n", command);
}

static void process_len_argv(
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

static void get_usage(char *command)
{
    printf("Usage: %s [options] get [options] <args>\n", command);
}

static void process_get_argv(char *command, char *path, size_t path_len, int argc, char **argv)
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
                idx = way_count_elems(path, path_len) - 1;
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