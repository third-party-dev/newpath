#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

void print_usage(const char *command)
{
    printf("Usage: %s [options] <subcommand> [options] <args>\n", command);
}

// insert
// delete
// count
// len
// get

#define UNKNOWN ('U')
#define INSERT ('I')
#define DELETE ('D')
#define COUNT ('C')
#define LEN ('L')
#define GET ('G')

char subcommand_to_code(const char *argv_elem) {
  if (strcmp(argv_elem, "insert") == 0) {
    return INSERT;
  }
  if (strcmp(argv_elem, "delete") == 0) {
    return DELETE;
  }
  if (strcmp(argv_elem, "count") == 0) {
    return COUNT;
  }
  if (strcmp(argv_elem, "len") == 0) {
    return LEN;
  }
  if (strcmp(argv_elem, "get") == 0) {
    return GET;
  }
  else { return UNKNOWN; }
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

#define STR_EQ(a, b) (strcmp(a, b) == 0)

void insert_elem(char *path, size_t path_len, int idx, char *npath) {
    size_t count = 0;
    int i;
    
    if (idx == 0) {
        printf("%s", npath);
        if (path_len > 0) printf(":");
    }

    // Get the count.
    for (i = 0; i < path_len; ++i) {
        printf("%c", path[i]);
        if (path[i] == ':') {
            ++count;
            if (count == idx) {
                printf("%s", npath);
                if (path_len - i > 0) printf(":");
            }
        }
    }

    if (count + 1 == idx) {
        if (path_len > 0) printf(":");
        printf("%s", npath);
    }
}

void process_insert_argv(char *command, char *path, size_t path_len, int argc, char **argv)
{
    int idx = 0;
    char *tgtpath = NULL;

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
            else if (STR_EQ(argv[i], "--index") || STR_EQ(argv[i], "-i")) {  
                if (i + 1 < argc) {
                    ++i;
                    // TODO: Need _better_ error check
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
            // !Other subcommand options go here.
            else {
                goto usage;
            }
            continue;
        }
        break;
    }

    // Process subcommand arguments
    if (argc - i == 1) {
        tgtpath = argv[i];
        //printf("Insert %s at %d\n", tgtpath, idx);
        insert_elem(path, path_len, idx, tgtpath);
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

void delete_elem(char *path, size_t path_len, int idx) {
    size_t count = 0;
    int i;

    for (i = 0; i < path_len; ++i) {
        if (path[i] == ':') ++count;
        if (path[i] == ':' && idx == count) continue;
        if (idx != count) printf("%c", path[i]);
    }
}

void process_delete_argv(char *command, char *path, size_t path_len, int argc, char **argv)
{
    int idx = 0;

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
            else if (STR_EQ(argv[i], "--index") || STR_EQ(argv[i], "-i")) {  
                if (i + 1 < argc) {
                    ++i;
                    // TODO: Need _better_ error check
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
            // !Other subcommand options go here.
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

    delete_elem(path, path_len, idx);
    exit(0);

usage:
    delete_usage(command);
    exit(1);
}

void count_usage(const char *command)
{
    printf("Usage: %s [options] count [options] <args>\n", command);
}

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
            // !Other subcommand options go here.
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
            // !Other subcommand options go here.
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

void get_elem(char *path, size_t path_len, int idx) {
    size_t count = 0;
    int i;

    for (i = 0; i < path_len; ++i) {
        if (path[i] == ':') ++count;
        if (path[i] == ':' && idx == count) continue;
        if (idx == count) printf("%c", path[i]);
    }
}

void process_get_argv(char *command, char *path, size_t path_len, int argc, char **argv)
{
    int idx = 0;
    char *sub;
    size_t sub_len;
    char *res;
    int i;

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
            else if (STR_EQ(argv[i], "--index") || STR_EQ(argv[i], "-i")) {  
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
            // !Other subcommand options go here.
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

    // Process subcommand arguments
    get_elem(path, path_len, idx);
    exit(0);

usage:
    get_usage(command);
    exit(1);
}

int main(int argc, char **argv)
{
    int i;
    
    int have_subcommand = 0;
    char *subcommand;
    char subcommand_code = 0;

    char *envpath;
    size_t envpath_len;

    envpath = getenv("PATH");
    envpath_len = strlen(envpath);

    // Usage: <command> [options] <sub command> [options] <args>
    // States:
    //   - Command
    //   - Command Options
    //   - Subcommand
    //   - Subcommand Options
    //   - Subcommand Arguments

    // 1. Get command
    char *command = argv[0];

    // 2. Process options until sub_command
    if (argc == 1)
    {
        printf("%s", envpath);
        exit(0);
    }

    for (i = 1; i < argc; ++i) {
        if (*argv[i] == '-')
        {
            if (strcmp(argv[i],"--help")) {
                print_usage(command);
                exit(1);
            }
            // !Other subcommand options go here.
            else {
                print_usage(command);
                exit(1);
            }
            continue;
        }
        else if (subcommand_code == 0 && *argv[i] != '-') {
            subcommand = argv[i];
            subcommand_code = subcommand_to_code(subcommand);
            ++i;
            break;
        }
    }

    switch (subcommand_code) {
        case INSERT:
            process_insert_argv(command, envpath, envpath_len, argc - i, argv + i);
            break;
        case DELETE:
            process_delete_argv(command, envpath, envpath_len, argc - i, argv + i);
            break;
        case COUNT:
            process_count_argv(command, envpath, envpath_len, argc - i, argv + i);
            break;
        case LEN:
            process_len_argv(command, envpath, envpath_len, argc - i, argv + i);
            break;
        case GET:
            process_get_argv(command, envpath, envpath_len, argc - i, argv + i);
            break;
    }

    print_usage(command);
    return 1;
}