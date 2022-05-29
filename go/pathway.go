package main

import (
	"flag"
	"fmt"
	"os"
	"strings"
)

// char argv_to_op(const char *argv_elem) {
// 	if (strcmp(argv_elem, "insert") == 0) {
// 	  return INSERT;
// 	}
// 	if (strcmp(argv_elem, "delete") == 0) {
// 	  return DELETE;
// 	}
// 	if (strcmp(argv_elem, "count") == 0) {
// 	  return COUNT;
// 	}
// 	if (strcmp(argv_elem, "len") == 0) {
// 	  return LEN;
// 	}
// 	if (strcmp(argv_elem, "get") == 0) {
// 	  return GET;
// 	}
// 	else { return UNKNOWN; }
//   }

func main() {
	println(os.Args[0])

	flag.Parse()
	subcommand := flag.String("env", "development", "current environment")
	opt := flag.Int("port", 3000, "port number")

	// Fetch PATH value
	var path_value string
	for _, v := range os.Environ() {
		slc := strings.SplitN(v, "=", 2)
		if slc[0] == "PATH" {
			path_value = slc[1]
			break
		}
	}

	fmt.Printf("Path: %s", path_value)

	// Slice PATH value
	path_slc := strings.Split(path_value, ":")
	for i, v := range path_slc {
		fmt.Println(i, v)
	}

	os.Exit(0)
}
