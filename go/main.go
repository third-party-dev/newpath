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

package main

import (
	"fmt"
	"os"
	"strconv"

	way "third-party.dev/git/projects/way/api"
)

func is_decimal(arg string) bool {
	for _, c := range arg {
		if c < '0' || c > '9' {
			return false
		}
	}
	return true
}

func is_hex(arg string) bool {
	// TODO: Is X valid?
	if (len(arg) < 3) || (arg[0] != '0') || (arg[1] != 'x' && arg[1] != 'X') {
		return false
	}

	i := 2
	for ; i < len(arg); i++ {
		if (arg[i] < '0' || arg[i] > '9') && (arg[i] < 'a' && arg[i] > 'f') && (arg[i] < 'A' && arg[i] > 'F') {
			return false
		}
	}
	return true
}

func is_octal(arg string) bool {
	// TODO: Is X valid?
	if (len(arg) < 2) || (arg[0] != '0') {
		return false
	}

	i := 1
	for ; i < len(arg); i++ {
		if arg[i] < '0' || arg[i] > '7' {
			return false
		}
	}
	return true
}

func is_valid_index(arg string) bool {
	return (is_decimal(arg) || is_hex(arg) || is_octal(arg))
}

func process_insert(command string, path string, args []string) {

	var idx uint64 = 0

	index_given := 0

	i := 0
	for ; i < len(args); i++ {
		if args[i] == "--" {
			i++
			break
		} else if args[i][0] == '-' {
			if args[i] == "--help" {
				goto usage
			} else if (args[i] == "-i" || args[i] == "--index") && index_given == 0 {
				index_given = 1
				if i+1 < len(args) {
					i++
					if is_valid_index(args[i]) {
						var errno error
						idx, errno = strconv.ParseUint(args[i], 0, 64)
						// TODO: Check for overflow and underflow. (How to use limits.h in Go?)
						if /*idx == limits.LONG_MIN || idx == limits.LONG_MAX ||*/ errno != nil {
							goto usage
						}
					} else {
						goto usage
					}
				}
			} else if (args[i] == "-t" || args[i] == "--tail") && index_given == 0 {
				index_given = 1
				idx = way.Count(path)
			} else {
				goto usage
			}
			continue
		}
		break
	}

	if len(args)-i == 1 {
		npath := args[i]
		way.InsertPrint(path, idx, npath)
		os.Exit(0)
	}

usage:
	fmt.Printf("Insert Usage: %s", path)
	os.Exit(1)
}

func process_delete(command string, path string, args []string) {
	var idx uint64 = 0

	index_given := 0

	i := 0
	for ; i < len(args); i++ {
		if args[i] == "--" {
			i++
			break
		} else if args[i][0] == '-' {
			if args[i] == "--help" {
				goto usage
			} else if (args[i] == "-i" || args[i] == "--index") && index_given == 0 {
				index_given = 1
				if i+1 < len(args) {
					i++
					if is_valid_index(args[i]) {
						var errno error
						idx, errno = strconv.ParseUint(args[i], 0, 64)
						// TODO: Check for overflow and underflow. (How to use limits.h in Go?)
						if /*idx == limits.LONG_MIN || idx == limits.LONG_MAX ||*/ errno != nil {
							goto usage
						}
					} else {
						goto usage
					}
				}
			} else if (args[i] == "-t" || args[i] == "--tail") && index_given == 0 {
				index_given = 1
				idx = way.Count(path)
				if idx > 0 {
					idx--
				}
			} else {
				goto usage
			}
			continue
		}
		break
	}

	if len(args)-i == 0 {
		way.DeletePrint(path, idx)
		os.Exit(0)
	}

usage:
	/* Dump path even during errors to prevent an error from busting PATH */
	fmt.Printf("%s", path)
	fmt.Printf("Usage: ...")
	//delete_usage(command);
	os.Exit(1)
}

func process_get(command string, path string, args []string) {
	var idx uint64 = 0

	index_given := 0

	i := 0
	for ; i < len(args); i++ {
		if args[i] == "--" {
			i++
			break
		} else if args[i][0] == '-' {
			if args[i] == "--help" {
				goto usage
			} else if (args[i] == "-i" || args[i] == "--index") && index_given == 0 {
				index_given = 1
				if i+1 < len(args) {
					i++
					if is_valid_index(args[i]) {
						var errno error
						idx, errno = strconv.ParseUint(args[i], 0, 64)
						// TODO: Check for overflow and underflow. (How to use limits.h in Go?)
						if /*idx == limits.LONG_MIN || idx == limits.LONG_MAX ||*/ errno != nil {
							goto usage
						}
					} else {
						goto usage
					}
				}
			} else if (args[i] == "-t" || args[i] == "--tail") && index_given == 0 {
				index_given = 1
				idx = way.Count(path)
				if idx > 0 {
					idx--
				}
			} else {
				goto usage
			}
			continue
		}
		break
	}

	if len(args)-i == 0 {
		way.GetPrint(path, idx)
		os.Exit(0)
	}

usage:
	fmt.Printf("Usage: ...")
	//get_usage(command);
	os.Exit(1)
}

func process_count(command string, path string, args []string) {

	i := 0
	for ; i < len(args); i++ {
		if args[i][0] == '-' {
			if args[i] == "--help" {
				goto usage
			} else {
				goto usage
			}
			continue
		}
		break
	}

	if len(args) != i {
		goto usage
	}

	fmt.Printf("%d", way.Count(path))
	os.Exit(0)

usage:
	//count_usage(command);
	fmt.Printf("Usage: ...")
	os.Exit(1)
}

func process_bytes(command string, path string, args []string) {

	i := 0
	for ; i < len(args); i++ {
		if args[i][0] == '-' {
			if args[i] == "--help" {
				goto usage
			} else {
				goto usage
			}
			continue
		}
		break
	}

	if len(args) != i {
		goto usage
	}

	fmt.Printf("%d", len(path))
	os.Exit(0)

usage:
	//count_usage(command);
	fmt.Printf("Usage: ...")
	os.Exit(1)
}

func process_chars(command string, path string, args []string) {

	count, i := 0, 0

	for ; i < len(args); i++ {
		if args[i][0] == '-' {
			if args[i] == "--help" {
				goto usage
			} else {
				goto usage
			}
			continue
		}
		break
	}

	if len(args) != i {
		goto usage
	}

	// Note: range implicitly decodes runes
	for i = range path {
		count++
	}

	fmt.Printf("%d", count)
	os.Exit(0)

usage:
	//count_usage(command);
	fmt.Printf("Usage: ...")
	os.Exit(1)
}

func process_meta() {
	version := `"version":"0.0.0"`
	syntax := `"syntax":"go"`
	options := `"options:":{"--help":"flag"}`
	commands := `"commands":{"insert":{},"delete":{},"get":{},"count":{},"bytes":{},"chars":{}}`
	fmt.Printf("{%s,%s,%s,%s}", version, syntax, options, commands)
	os.Exit(0)
}

func main() {
	var subcommand string

	path := os.Getenv("PATH")

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
	//command := os.Args[0]

	// https://github.com/famz/SetLocale/blob/master/SetLocale.go
	// C.setlocale(0, "")
	/* 2. Process options until sub_command */
	if len(os.Args) <= 1 {
		fmt.Printf("%s", path)
		os.Exit(0)
	}

	//offset := 0
	i := 1
	for ; i < len(os.Args); i++ {
		//for i, v := range os.Args[1:] {
		if os.Args[i][0] == '-' {
			if os.Args[i] == "--help" {
				goto usage
			} else {
				goto usage
			}
		} else if subcommand == "" && os.Args[i][0] != '-' {
			subcommand = os.Args[i]
			i++
			break
		}
	}

	if subcommand != "" {
		if subcommand == "insert" {
			process_insert(os.Args[0], path, os.Args[i:])
		} else if subcommand == "delete" {
			process_delete(os.Args[0], path, os.Args[i:])
		} else if subcommand == "count" {
			process_count(os.Args[0], path, os.Args[i:])
		} else if subcommand == "bytes" {
			process_bytes(os.Args[0], path, os.Args[i:])
		} else if subcommand == "chars" {
			process_chars(os.Args[0], path, os.Args[i:])
		} else if subcommand == "get" {
			process_get(os.Args[0], path, os.Args[i:])
		} else if subcommand == "meta" {
			process_meta()
		}
	}

	// TODO: Consider path output options?

usage:
	fmt.Printf("%s", path)
	fmt.Printf("Usage: ...")
	//print_usage(command);
	os.Exit(1)

}
