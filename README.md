# PATH=\`way\`

[![build](https://github.com/third-party-dev/pathway/actions/workflows/c89.yaml/badge.svg)](https://github.com/third-party-dev/pathway/actions)

Pathway (aka "Way") is a utility to assist with changing the PATH environment variable, or any `:` separated search path.

## Overview

I'm often switching between different versions of compiler toolchains and other software by changing my shell's $PATH. This could be to trick some unknowing software into using a different version of software or it could be so that I don't have to type in long absolute paths.

When I change the path the traditional means, it usually means I'm typing something like: `export PATH=$PATH:/new/path`. This is error prone because if I accidentally mistype the `$PATH` part, I can accidentally wipe my existing `$PATH` settings. 

Also, sometimes I want to surgically replace or remove a specific path from `$PATH` that isn't at the beginning or end of `$PATH`. When this happens, I can attempt to be cleaver by constructing a sed command to remove/replace the folder, or more commonly do two copy/paste actions into a subsequent command. Both of these are quite error prone.

## Build It

Assuming you have a GCC compiler installed in your system:

```text
git clone https://github.com/third-party-dev/pathway
cd pathway/c89
./build.sh
```

You can optionally perform a user install with: `./user-install.sh`. Note: At the moment, you'll need to manually add `$HOME/.local/bin` to your `$PATH`.

## Use Way To Simplify Path Modifications

In the case where you want to add a new path to `$PATH`, its as simple as:

```sh
export PATH=`way insert /new/path`
```

`way` is a binary that does one thing and aims to do it well. It has minimal dependencies and has been deliberately written in C89 to be build-able for as many environments as possible (forward and backward in time).

This technique prevents me from accidentally misspelling `$PATH`. In the event that an invalid set of arguments are passed to `way insert`, `way` will still automatically output the original `$PATH` so that a typo or mistake doesn't leave you with lost settings. Note: Errors and usage messages are output to STDERR while the `$PATH` output is sent to STDOUT.

### Modifying Environment From Child Process

`way`, just like any separate process, suffers from the limitation that a child process can not modify it's parent's environment. Therefore all of the following examples will include some sort of ``export PATH=`command` ``. We can very simply generate shell scripts or aliases for the specific shell that abstract away the _export_ and _PATH_ noise in the command, but `way` has been designed to not be coupled to any specific shell itself.

Although `way` has no intention of being tightly bound to a shell, the development road map does include canned shell configurations (bash/ksh/zsh/tcsh scripts) and built-in integrations for a number of environments (e.g. busybox/dash/bash).

## "But I Can Already Do That With My Shell!"

There are obviously multiple ways to get this job done, and here are a few I've used:

Use Case: Append to PATH

```text
# Without way:
export PATH=$PATH:/new/path
# With way:
export PATH=`way insert -t /new/path`
```

Use Case: Prepend to PATH

```text
# Without way:
export PATH=/new/path:$PATH
# With way:
export PATH=`way insert /new/path`
```

Use Case: Remove A Path from PATH

```text
# Without way:
export PATH=`echo $PATH | tr ":" "\n" | grep -v "/three" | tr "\n" ":"`
# With way:
export PATH=`way list | grep -v /three | way join`
```

Use Case: Replace A Path

```text
# Given: PATH=/four:/two:/three
# Without way:
export PATH=`echo -ne $PATH | tr ":" "\n" | sed 's~^/one$~/four~' | tr "\n" ":"`
# With way:
export PATH=`way replace /four`
```

## Way Supports Pipes

You can chain way with other tools. For example, you can pipe several `way` calls together:

```test
# Given: PATH=/1:/2:/3
# Replace values with: 
PATH=`way replace -i 0 /one | way replace -i 1 /two | way replace -i 2 /three`
# Output
PATH=/one:/two:/three
```

# Interfaces

## CLI Commands

### `way`

Reads and dumps the `$PATH` to STDOUT. If a stream is piped into `way`, it dumps the STDIN to STDOUT.

### `way insert`

Inserts a path into $PATH. 

Without any arguments it will prepend the given path to the beginning of $PATH.

- Given: `PATH=/one:/two:/three`
- Run: `way insert /four`
- Output: `/four:/one:/two:/three`

If you provide an index, you can insert the element anywhere in the array. It always inserts the given path before the indexed element.

- Given: `PATH=/one:/two:/three`
- Run: `way insert -i 2 /four`
- Output: `/one:/two:/four:/three`

If you want to append a path to the end, you can tell `way` to use the _tail_.

- Given: `PATH=/one:/two:/three`
- Run: `way insert -t /four`
- Output: `/one:/two:/three:/four`

### `way delete`

Removes a path from $PATH. 

Without any arguments it will remove the first path in $PATH.

- Given: `PATH=/one:/two:/three`
- Run: `way delete`
- Output: `/two:/three`

If you provide an index, you can remove any valid element in the array.

- Given: `PATH=/one:/two:/three`
- Run: `way delete -i 1 /four`
- Output: `/one:/three`

If you want to remove the last path from $PATH, tell `way` to use the _tail_.

- Given: `PATH=/one:/two:/three`
- Run: `way delete -t`
- Output: `/one:/two`

### `way replace`

Replaces a path in $PATH. 

Without any arguments it will replace the first path in $PATH.

- Given: `PATH=/one:/two:/three`
- Run: `way replace /four`
- Output: `/four:/two:/three`

Provide an index to replace any element in the array.

- Given: `PATH=/one:/two:/three`
- Run: `way replace -i 1 /four`
- Output: `/one:/four:/three`

To replace the final path, tell `way` to use the _tail_.

- Given: `PATH=/one:/two:/three`
- Run: `way replace -t /four`
- Output: `/one:/two:/four`

### `way count`

Counts elements in the $PATH array.

- Given: `PATH=/one:/two:/three`
- Run: `way count`
- Output: `3`

### `way list`

List elements line separated.

- Given: `PATH=/one:/two:/three`
- Run: `way list`
- Output: 

  ```text
  /one
  /two
  /three
  ```

### `way join`

Join line separated elements in a PATH value.

- Run: `echo -en "/one\n/two\n/three" | way join`
- Output: `/one:/two:/three`

### `way chars`

Counts the multi-byte characters in $PATH (dependent on system's locale).

- Given: `PATH=/one:/🍕:/three`
- Run: `way chars`
- Output: `14`

### `way bytes`

Counts the bytes in $PATH (i.e. C/POSIX locale).

- Given: `PATH=/one:/🍕:/three`
- Run: `way bytes`
- Output: `17`

## Way C-API

`way` is built with the Way C-API. The API is completely decoupled from the CLI and can be used as a static or shared library in other projects. The following is a listing of the public calls in the library.

Note: The development road map includes ported API support for a number of languages syntaxes, including Go, Python, and Node.

<details><summary>Click to View API</summary>

```c
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


/* List elements on lines from given memory range to stream. */
void way_list_mem2fd(
    int out_fd,
    char *path,
    size_t path_len);


/* List elements on lines from given stream to stream. */
void way_list_fd2fd(
    int out_fd,
    int in_fd);


/* Join whitespace delimited elements from given memory range to stream. */
void way_join_mem2fd(
    int out_fd,
    char *path,
    size_t path_len);


/* Join whitespace delimited elements from given stream to stream. */
void way_join_fd2fd(
    int out_fd,
    int in_fd);
```

</details>

## History

### A Quick -n- Brief history of PATH

Before the big bang (i.e. Unix), there was one of the first and more popular time-sharing operating systems Multics (Multiplexed Information and Computing Service). _Multics first introduced a hierarchical file system with directories (separated by ">") in the mid-1960s. Around 1970, Unix introduced the slash character ("/") as its directory separator._ -[Wikipedia / Path](https://en.wikipedia.org/wiki/Path_(computing))

In the beginning there was the [Thompson shell](https://en.wikipedia.org/wiki/Thompson_shell) (circa 1971). This is the Multics (precursor to Unix) shell that originally introduced input/output redirection, pipes.

_Multics originated the idea of a search path. The early Unix shell only looked for program names in /bin, but by Version 3 Unix the directory was too large and /usr/bin, and a search path, became part of the operating system.[1]Multics originated the idea of a search path. The early Unix shell only looked for program names in /bin, but by Version 3 Unix the directory was too large and /usr/bin, and a search path, became part of the operating system._ -[Wikipedia / PATH](https://en.wikipedia.org/wiki/PATH_(variable))

Next, the PWB Shell came onto the scene as a proper Unix shell. This shell introduced conditional logic (if/else/switch/while). 

  _The PWB shell or Mashey shell, sh, was an upward-compatible version of the Thompson shell, augmented by John Mashey and others and distributed with the Programmer's Workbench UNIX, circa 1975–1977. It focused on making shell programming practical, especially in large shared computing centers. **It added shell variables (precursors of environment variables, including the search path mechanism that evolved into $PATH)**, user-executable shell scripts, and interrupt-handling. Control structures were extended from if/goto to if/then/else/endif, switch/breaksw/endsw, and while/end/break/continue. As shell programming became widespread, these external commands were incorporated into the shell itself for performance._ -[Wikipedia / Unix Shell](https://en.wikipedia.org/wiki/Unix_shell) 

  _The $p variable was the ancestor of $PATH, which let users search for commands in their own choice of directories. Unlike most of the UNIX systems of the time, the original PWB/UNIX computer center was shared by multiple programming groups who could not change the contents of /bin or /usr/bin, but wanted to create their own sets of shared commands. In addition, the shell's command-searching was enhanced to allow shell procedures to be invoked like binary commands, i.e., if the shell found a non-binary file marked executable, it would fork another shell instance to read that file as a shell script. Thus people could type command arguments rather than sh pathname/command arguments. All this behavior was packaged as the function pexec, which was the ancestor of execvp, to allow any program to invoke commands in the same way as the shell._ -[Wikipedia / PWB Shell](https://en.wikipedia.org/wiki/PWB_shell)

With the lessons learned from PWB Shell and Thompson shell, Steven Bourne started to develop a new shell for [Unix version 7](https://en.wikipedia.org/wiki/Version_7_Unix).

  _This Bourne shell was incompatible with the Thompson and PWB shells, but included equivalents of most of the PWB shell's features, but done from scratch, rather than incrementally, with much discussion among the various participants. In particular, environment variables and related machinery were designed by Stephen Bourne, John Mashey, and Dennis Ritchie as a general mechanism to replace the earlier, more limited features_ -[Wikipedia / PWB Shell](https://en.wikipedia.org/wiki/PWB_shell)

  _On POSIX and Unix-like operating systems, the $PATH variable is specified as a list of one or more directory names separated by colon (:) characters. **Directories in the PATH-string are not meant to be escaped, making it impossible to have directories with : in their name.**_ -[Wikipedia / PATH](https://en.wikipedia.org/wiki/PATH_(variable))

### Stephen Collyer's Path Functions

Articles are from ~2000:

- [Shell Functions and Path Variables, Part 1](https://www.linuxjournal.com/article/3645)
- [Shell Functions and Path Variables, Part 2](https://www.linuxjournal.com/article/3768)
- [Shell Functions and Path Variables, Part 3](https://www.linuxjournal.com/article/3935)
- [pathfunc.tgz](https://web.archive.org/web/20061210054813/http://www.netspinner.co.uk:80/Downloads/pathfunc.tgz)

`pathfunc.tgz` Take Aways:

- Existing implementation dependent on Bash or Ksh.
- Add paths to front and back of PATH.
- Edit PATH in editor.
- Make PATH from list in $@ (i.e. argv).
- Make elements of PATH unique.
- Split/join PATH elements to/from lines.
- Delete path based on regular expression.

# Development Road Map Thoughts

## Feature Wishlist

- Regular Expressions
- Encoding Specifications
- Single Command, Multiple Actions
- User Configuration Overrides (~/.wayrc)
- More exhaustive unit tests
- Swap subcommand
- De-duplication

## Language Ports

- C
  - Built with C89. isatty() is the only unistd call.
  - Build with `./build.sh`.
  - `way.c`/`way.h` constitute the API and reusable Library.
  - `cli.c` constitutes the CLI application of the way API.
  - `cppref.h` is a code snippet reused from cppreference.com.

- Go
  - Developed and Tested with Go 1.18
  - Build without stream support.

- Python 3
  - Developed and Tested with Python 3.7
  - Built without stream support.

## Future Language Ports

- Python
  - Python 2.3-7, 3.2-7, 3.latest

- Javascript
  - Develop and Test with Node 8-16

- Java
  - Develop and Test with JDK11 (or JDK8)

- Rust?

- Kotlin?

- C++?

## Future Shell BuiltIn Integrations

- Bash, Dash, Busybox

## Future Shell Configurations

- Bash, Zsh, Tcsh, Ksh

## Future Package Management

- deb, ppa (Ubuntu, Debian)
- rpm (Redhat, Centos, Fedora)
- pip, pip3 (Python distributions)
- npm, yarn
- Go Module?
- Rust?
- CMake?
- JFrog Conan Binary Packages
- Maven?
- Docker (because I think its funny)

# Edge Cases

## Weird Paths

`/bin:` / `:/bin` - A _naked_ separator in the PATH typically includes the _current_ path (i.e. \`pwd\`).

"" - `way` views an empty PATH as valid and an undefined element as an empty PATH. (`count == 0`, `bytes == 0`)

`:` - `way` views a single separator as 2 empty elements. (`count == 2`, `bytes == 1`)

`::` - `way` views two sole separators as 3 empty elements. (`count == 3`, `bytes == 2`)

## Compute Limits

See limits: `true | xargs --show-limits`

```text
Your environment variables take up 3232 bytes
POSIX upper limit on argument length (this system): 2091872
POSIX smallest allowable upper limit on argument length (all systems): 4096
Maximum length of command we could actually use: 2088640
Size of command buffer we are actually using: 131072
Maximum parallelism (--max-procs must be no greater): 2147483647
```

See ARG_MAX: `getconf -a | grep ARG_MAX`

```text
ARG_MAX                            2097152
_POSIX_ARG_MAX                     2097152
```

From execve man page:

```text
Limits on size of arguments and environment

    Most UNIX implementations impose some limit on the total size of
    the command-line argument (argv) and environment (envp) strings
    that may be passed to a new program.  POSIX.1 allows an
    implementation to advertise this limit using the ARG_MAX constant
    (either defined in <limits.h> or available at run time using the
    call sysconf(_SC_ARG_MAX)).

    On Linux prior to kernel 2.6.23, the memory used to store the
    environment and argument strings was limited to 32 pages (defined
    by the kernel constant MAX_ARG_PAGES).  On architectures with a
    4-kB page size, this yields a maximum size of 128 kB.

    On kernel 2.6.23 and later, most architectures support a size
    limit derived from the soft RLIMIT_STACK resource limit (see
    getrlimit(2)) that is in force at the time of the execve() call.
    (Architectures with no memory management unit are excepted: they
    maintain the limit that was in effect before kernel 2.6.23.)
    This change allows programs to have a much larger argument and/or
    environment list.  For these architectures, the total size is
    limited to 1/4 of the allowed stack size.  (Imposing the
    1/4-limit ensures that the new program always has some stack
    space.)  Additionally, the total size is limited to 3/4 of the
    value of the kernel constant _STK_LIM (8 MiB).  Since Linux
    2.6.25, the kernel also places a floor of 32 pages on this size
    limit, so that, even when RLIMIT_STACK is set very low,
    applications are guaranteed to have at least as much argument and
    environment space as was provided by Linux 2.6.22 and earlier.
    (This guarantee was not provided in Linux 2.6.23 and 2.6.24.)
    Additionally, the limit per string is 32 pages (the kernel
    constant MAX_ARG_STRLEN), and the maximum number of strings is
    0x7FFFFFFF.
```

Microsoft Environment Limits:

  - Environment limited to ~32KB
  - Individual variables may be limited to ~2K.
  - [What is the maximum length of an environment variable?](https://devblogs.microsoft.com/oldnewthing/20100203-00/?p=15083#:~:text=The%20theoretical%20maximum%20length%20of,a%20limit%20of%2032767%20characters.)

### Relevance

`way` stream support currently limits a set of operations to memory only operations because it doesn't attempt to record or rewind any of the stream its processing. This keeps memory usage low. 

If `way` attempted to capture a stream into memory to re-enable this functionality, it could make the expected results non-deterministic for extreme environments and it would make the implementation more complex. If this is attempted in the future, it'd likely be opt-in only (i.e. another required flag or user config setting).