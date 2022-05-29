#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#define UNKNOWN ('U')
#define INSERT ('I')
#define DELETE ('D')
#define COUNT ('C')
#define LEN ('L')
#define GET ('G')

char argv_to_op(const char *argv_elem) {
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

void print_usage(const char *exe) {
  printf("Usage: %s [insert|delete|count|len|get] [args]\n", exe);
}

size_t count_path_elems(const char *path) {
  size_t path_len = 0;
  size_t count = 0;
  int i = 0;
  
  path_len = strlen(path);
  
  for (i = 0; i < path_len; ++i) {
    if (path[i] == ':') {
      count += 1;
    }
  }
  
  return count + 1;
}

#define IS_PORTABLE_FNAME_CHAR(c) \
 ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '.' || c == '-' || c == '_' || c == '/' || c == ':')

int is_portable(const char *path, const size_t path_len)
{
  size_t i;
  for (i = 0; i < path_len; ++i)
  {
    if (!IS_PORTABLE_FNAME_CHAR(path[i])) {
      printf("Bad character: %c\n", path[i]);
      return 0;
    }
  }
  return 1;
}

void get_elem(char *path, size_t path_len, int idx, char **sub, size_t *sub_len)
{
  char *end = path + path_len;
  char *pptr = path;
  char *nptr;
  unsigned int elem = 0;
  
  if (sub) *sub = NULL;
  if (sub_len) *sub_len = 0;
  if (path == NULL) return;
  
  while ( pptr != NULL)
  {
    if (pptr == end && *pptr == ':') {
      if (elem == idx) {
        if (sub) *sub = end;
        if (sub_len) *sub_len = 0;
      }
      break;
    }
    
    nptr = strchr(pptr, ':');
    
    if (nptr != NULL) {
      char *ptr;
      
      if (elem == idx) {
        if (sub) *sub = pptr;
        if (sub_len) *sub_len = (size_t)nptr - (size_t)pptr;
      }
      
      pptr = nptr != end ? pptr = nptr + 1 : nptr;
      elem += 1;
      
      continue;
    }
    else {
      char *ptr;
      
      if (elem == idx) {
        if (sub) *sub = pptr;
        if (sub_len) *sub_len = (size_t)end - (size_t)pptr;
      }
      
      pptr = nptr;
    }
  }
}

#define IS_DECIMAL(c) (c >= '0' && c <= '9')
int get_index_arg(const char *arg, size_t *idx)
{
  size_t i;
  size_t arg_len = strlen(arg);
  
  if (idx) *idx = (size_t) -1;
  
  for (i = 0; i < arg_len; ++i) {
    if (!IS_DECIMAL(arg[i])) return 1;
  }
  
  errno = 0;
  if (idx) *idx = strtol(arg, NULL, 10);
  if (errno) return 1;
}

int main(int argc, char **argv)
{
  char *original;
  size_t original_len;
  char operation;
  
  if (argc == 1) {
    print_usage(argv[0]);
    return 1;
  }
  
  if (argc >= 2) {
    operation = argv_to_op(argv[1]);
  }
  
  original = getenv("PATH");
  origina_len = strlen(original);
  
  if (!is_portable(original, original_len)) {
    printf("Error: PATH not portable.\n");
    print_usage(argv[0]);
    return 1;
  }
  
  if (operation == LEN) {
    if (argc != 2) {
      print_usage(argv[0]);
      return 1;
    }
    
    if (origina == NULL || original_len == 0) {
      printf("0");
      return 0;
    }
    
    printf("%u", (unsigned int)original_len);
    return 0;
  }
  
  if (operation == COUNT) {
    if (argc != 2) {
      print_usage(argv[0]);
      return 1;
    }
    
    if (original == NULL || original_len == 0) {
      printf("0");
      return 0;
    }
    
    printf("%u", (unsigned int)count_path_elems(original));
    return 0;
  }
  
  if (operation == GET) {
    size_t idx;
    int ret;
    char *sub;
    size_t sub_len;
    char *res;
    
    if (argc != 3) {
      print_usage(argc[0]);
      return 1;
    }
    
    if (get_index_arg(argv[2], &idx))
    {
      printf("Bad index arg: %s\n", argv[2]);
      return 1;
    }
    
    get_elem(original, original_len, idx, &sub, &sub_len);
    
    if (original == NULL || original_len == 0 || sub == NULL || sub_len == 0) {
      return 0;
    }
    
    res = malloc(sub_len + 1);
    strncpy(res, sub, sub_len);
    sub[sub_len] = 0;
    printf("%s", res);
    return 0;
  }
  
  print_usage(argv[0]);
  return 1;
}
