#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <locale.h>

#include "way.h"

#define STR_EQ(s1, s2) (strcmp(s1, s2) == 0)

static void print_path(
    const char *path,
    const size_t path_len) {
    size_t i;
    for (i = 0; i < path_len; ++i) putchar(path[i]);
}

static void print_usage(
    const char *command)
{
    fprintf(stderr, 
        "Usage: %s [options] <subcommand> [subcommand options] <args>\n"
        "\n"
        "Tool for manipulation of PATH environment variable."
        "\n"
        "Options:\n"
        "\n"
        "  --help - Show this usage help.\n"
        "\n"
        "Sub Commands:\n"
        "\n"
        "  bytes - Output number of bytes in PATH.\n"
        "  chars - Output number of UTF-8 characters in PATH.\n"
        "  get - Get the PATH element at index. (default: 0)\n"
        "  insert - Insert the PATH element before index. (default: 0)\n"
        "  delete - Delete the PATH element at index. (default: 0)\n"
        "\n"
        "Note: Use --help with sub commands for more information.\n"
        "\n", command);
}

#define IS_DECIMAL(c) (c >= '0' && c <= '9')
static int is_decimal(
    const char *str) {
    int i = 0;
    for (i = 0; i < strlen(str); ++i) {
        if (!IS_DECIMAL(str[i])) return 0;
    }
    return 1;
}

#define IS_HEX(c) ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))
static int is_hex(
    const char *str) {
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
static int is_octal(
    const char *str) {
    int i;

    if (strlen(str) < 2) return 0;
    if (str[0] != '0') return 0;
    
    for (i = 1; i < strlen(str); ++i) {
        if (!IS_HEX(str[i])) return 0;
    }
    
    return 1;
}

#define IS_VALID_INDEX(argv, i) \
    (is_decimal(argv[i]) || is_hex(argv[i]) || is_octal(argv[i]))

#define IS_INDEX_ARG(argv, i) \
    (STR_EQ(argv[i], "--index") || STR_EQ(argv[i], "-i"))
#define IS_TAIL_ARG(argv, i) \
    (STR_EQ(argv[i], "--tail") || STR_EQ(argv[i], "-t"))


static void insert_usage(
    char *command)
{
    fprintf(stderr,
        "Usage: %s [options] insert [insert options] <args>\n"
        "\n"
        "Insert a path before element offset within PATH environment variable.\n"
        "\n"
        "Options:\n"
        "\n"
        "  --help - Show this usage help.\n"
        "  --index <value>, -i <value> - Specify insertion index. (default: 0)\n"
        "  --tail, -t - Insert the path at end of PATH.\n"
        "\n", command);
}

static void process_insert_argv(
    char *command,
    char *path,
    size_t path_len,
    int argc,
    char **argv)
{
    int idx = 0;
    char *tgtpath = NULL;

    int index_given = 0;

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
            else if (IS_INDEX_ARG(argv, i) && !index_given) {  
                index_given = 1;
                if (i + 1 < argc) {
                    ++i;
                    if (IS_VALID_INDEX(argv, i)) {
                        idx = strtol(argv[i], NULL, 0);
                        if (idx == LONG_MIN || idx == LONG_MAX || errno == ERANGE) {
                            goto usage;
                        }
                    } else {
                        goto usage;
                    }
                }
            }
            else if (IS_TAIL_ARG(argv, i) && !index_given) {
                index_given = 1;
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
    /* Dump path even during errors to prevent an error from busting PATH */
    print_path(path, path_len);
    insert_usage(command);
    exit(1);
}

static void delete_usage(
    char *command)
{
    fprintf(stderr,
        "Usage: %s [options] delete [delete options] <args>\n"
        "\n"
        "Delete path at given element offset within PATH environment variable.\n"
        "\n"
        "Options:\n"
        "\n"
        "  --help - Show this usage help.\n"
        "  --index <value>, -i <value> - Specify deletion index. (default: 0)\n"
        "  --tail, -t - Delete the path at end of PATH.\n"
        "\n", command);
}

static void process_delete_argv(
    char *command,
    char *path,
    size_t path_len,
    int argc,
    char **argv)
{
    int idx = 0;

    int index_given = 0;

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
            else if (IS_INDEX_ARG(argv, i) && !index_given) {
                index_given = 1;
                if (i + 1 < argc) {
                    ++i;
                    /* TODO: Need _better_ error check */
                    if (IS_VALID_INDEX(argv, i)) {
                        idx = strtol(argv[i], NULL, 0);
                        if (idx == LONG_MIN || idx == LONG_MAX || errno == ERANGE) {
                            goto usage;
                        }
                    } else {
                        goto usage;
                    }
                }
            }
            else if (IS_TAIL_ARG(argv, i) && !index_given) {
                index_given = 1;
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
    /* Dump path even during errors to prevent an error from busting PATH */
    print_path(path, path_len);
    delete_usage(command);
    exit(1);
}

static void count_usage(
    const char *command)
{
    fprintf(stderr,
        "Usage: %s [options] count [count options]\n"
        "\n"
        "Count path elements within PATH environment variable.\n"
        "\n"
        "Options:\n"
        "\n"
        "  --help - Show this usage help.\n"
        "\n", command);
}

static void process_count_argv(
    const char *command,
    char *path,
    size_t path_len,
    int argc,
    char **argv)
{
    int i;
    for (i = 0; i < argc; ++i) {
        if (*argv[i] == '-')
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

static void bytes_usage(
    const char *command)
{
    fprintf(stderr,
        "Usage: %s [options] bytes [bytes options]\n"
        "\n"
        "Prints number of bytes in PATH environment variable.\n"
        "\n"
        "Options:\n"
        "\n"
        "  --help - Show this usage help.\n"
        "\n", command);
}

static void process_bytes_argv(
    const char *command, 
    const char *path, 
    const size_t path_len, 
    int argc, 
    char **argv)
{
    int i;
    for (i = 0; i < argc; ++i) {
        if (*argv[i] == '-')
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
    bytes_usage(command);
    exit(1);
}

static void chars_usage(
    const char *command)
{
    fprintf(stderr,
        "Usage: %s [options] chars [chars options]\n"
        "\n"
        "Prints number of UTF-8 characters in PATH environment variable.\n"
        "\n"
        "Options:\n"
        "\n"
        "  --help - Show this usage help.\n"
        "\n", command);
}

static void process_chars_argv(
    const char *command, 
    const char *path, 
    const size_t path_len, 
    int argc, 
    char **argv)
{
    int i;
    for (i = 0; i < argc; ++i) {
        if (*argv[i] == '-')
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

    printf("%u", (unsigned int)way_count_chars(path, path_len));
    exit(0);

usage:
    chars_usage(command);
    exit(1);
}

static void get_usage(
    char *command)
{
    fprintf(stderr,
        "Usage: %s [options] get [get options]\n"
        "\n"
        "Print path element at index for PATH environment variable.\n"
        "\n"
        "Options:\n"
        "\n"
        "  --help - Show this usage help.\n"
        "  --index <value>, -i <value> - Specify selection index. (default: 0)\n"
        "  --tail, -t - Select the path element at end of PATH.\n"
        "\n", command);
}

static void process_get_argv(
    char *command,
    char *path,
    size_t path_len,
    int argc,
    char **argv)
{
    int idx = 0;
    char *sub;
    size_t sub_len;
    char *res;
    int i;
    int index_given = 0;

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
            else if (IS_INDEX_ARG(argv, i) && !index_given) {  
                index_given = 1;
                if (i + 1 < argc) {
                    ++i;
                    if (IS_VALID_INDEX(argv, i)) {
                        idx = strtol(argv[i], NULL, 0);
                        if (idx == LONG_MIN || idx == LONG_MAX || errno == ERANGE) {
                            goto usage;
                        }
                    } else {
                        goto usage;
                    }
                }
            }
            else if (IS_TAIL_ARG(argv, i) && !index_given) { 
                index_given = 1; 
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

int main(
    int argc,
    char **argv)
{
    int i;
    
    int have_subcommand = 0;
    char *subcommand = NULL;

    char *path = getenv("PATH");
    size_t path_len = strlen(path);

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

    setlocale(LC_ALL, "");

    /* 2. Process options until sub_command */
    if (argc == 1)
    {
        printf("%s", path);
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
            process_insert_argv(command, path, path_len, argc - i, argv + i);
        }
        else if (STR_EQ(subcommand, "delete")) {
            process_delete_argv(command, path, path_len, argc - i, argv + i);
        }
        else if (STR_EQ(subcommand, "count")) {
            process_count_argv(command, path, path_len, argc - i, argv + i);
        }
        else if (STR_EQ(subcommand, "bytes")) {
            process_bytes_argv(command, path, path_len, argc - i, argv + i);
        }
        else if (STR_EQ(subcommand, "chars")) {
            process_chars_argv(command, path, path_len, argc - i, argv + i);
        }
        else if (STR_EQ(subcommand, "get")) {
            process_get_argv(command, path, path_len, argc - i, argv + i);
        }
    }

    /* TODO: Consider path output options? */

usage:
    print_path(path, path_len);
    print_usage(command);
    return 1;
}