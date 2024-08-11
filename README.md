# Filez

A couple of file tools for the Unix shell.

 * [Duplicates](#duplicates) -- Find duplicate files.
 * [Variations](#variations) -- Find (meta-)data variations.
 * [Deduplicate](#deduplicate) -- Copy tree with hard links instead of duplicate files.
 * [Incremental](#incremental) -- Incremental backup with multiple search strategies.
 * [Link First Node](#link-first-node) -- Copies only the first hard link to a new tree.
 * [Tree Struct Diff](#tree-struct-diff) -- Compares structure of two file-and-directory trees.

The [*smart hash*](#the-smart-hash) used by these tools is explained at [the end of this file](#the-smart-hash).

Implemented in C++20 using the C and C++ standard libraries and Posix standard functions.

## Building

Requires a Unix system with GNU make and a C++20 capable Clang or GCC.

Enter `make` in the main directory, a successful run generates the binaries in `build/bin`.

Tested with Clang 14 from Xcode 14 under macOS 13 on ARM64, and with GCC 12 under Linux on x86-64.

### Linux

Please consult your distribution's manual on how to install GCC.
If the version included with your distribution does not have enough C++20 support you might need to search the web for other solutions.

### macOS

If the `clang++` included with your Xcode does not support C++20 you will need something more recent, for example using [Homebrew](https://brew.sh) to install the latest LLVM.

### Others

If you are trying to build this on any other Unix or Unix-like system, for example one of the BSDs, you probably know how to install or build yourself a suitable compiler...

## The Tools

None of these tools ever delete or overwrite any existing filesystem object!

### Duplicates

```
Usage: build/bin/duplicates [OPTION]... DIRECTORY [DIRECTORY]...
  Finds duplicate files in one or more directories.
  Files are duplicates when they have the same...
    -n   file name.
    -N   file name and size.
    -i   device and inode mode 1.
    -I   device and inode mode 2.
    -h   smart hash and size (default).
    -H   total hash and size.
    -x   file name, smart hash and size.
    -X   file name, total hash and size.
  Additional options are...
    -R   to change to non-recursive scanning.
    -C   to disable normalising the given paths.
  Special files like devices and pipes are ignored.
  The smart hash only hashes two or three small chunks
    when the file is large and the extension is one for
    which a partial hash is usually sufficient.
  The details are in hash_file.hpp and hash_size.hpp.
```

### Variations

```
Usage: build/bin/variations [OPTION]... DIRECTORY [DIRECTORY]...
  Finds file meta data variations in one or more directories.
    -s   Finds variations of file size for the same file name.
    -i   Finds variations of file name for the same device and inode.
    -n   Finds variations of smart hash for the same file name (default).
    -N   Finds variations of total hash for the same file name.
    -h   Finds variations of file name for the same smart hash.
    -H   Finds variations of file name for the same total hash.
    -x   Finds variations of device and inode for the same smart hash.
    -X   Finds variations of device and inode for the same total hash.
  Additional options are...
    -R   to change to non-recursive scanning.
    -C   to disable normalising the given paths.
```

### Deduplicate

```
Usage: build/bin/deduplicate [options]... <source_dir> <merged_dir>
  Creates a new directory hierarchy under merged_dir that mirrors source_dir.
  Directories are newly created. Files are hard-linked, not copied, such that
  when source_dir contains multiple identical copies of a file then all of the
  hard-linked versions in merged_dir will link to the same randomly chosen
  version of the file in source_dir.
  Files in the source dir are considered identical when...
    -h   the file size and smart hash match.
    -H   the file size and total hash match.
    -c N Copy instead of hard link all files smaller than N bytes, default 0.
  Source and merged dir must be on the same filesystem. Merged dir must not exist.
  Exactly one of -h and -H must be given.
```

### Incremental

```
Usage: build/bin/incremental [options]... <source_dir> [old_backup]... <new_backup>
  Creates a new directory hierarchy under new_backup that mirrors source_dir.
  Hard links files from the old_backups into new_backup when possible, copies
  the files from source_dir when the file does not exist in a previous backup.
  Files in the old backup(s) are hard linked instead of copied when...
    -h   the file size and smart hash match.
    -H   the file size and total hash match.
    -n   the file size and file name and smart hash match.
    -N   the file size and file name and total hash match.
    -p   the file size and file name match.
    -P   the file size and relative path within source_dir and the old_backup dir match, including file name.
    -x   Consider freshly copied files as candidates for hard linking.
    -c N Copy instead of hard link all files smaller than N, default 0.
  Options can be combined, e.g. -hP (or -h -P) will search according to both -h and -P.
  At least one option different from -c must be given -- though everything except -x is a nop without an old_backup.
```

### Link First Node

```
Usage: link_first_node <source_dir> <sparse_dir>
  Creates a new directory hierarchy under sparse_dir that partially mirrors
  source_dir. Files under source_dir are hard-linked correspondingly into
  sparse_dir only if no other hard-link to the same inode was already created
  during the current operation, and directories are created under sparse_dir
  as required by the hard-linked files. Which of multiple paths from source_dir
  sharing the same inode will be created under sparse_dir is unspecified.
  Source and sparse dir must be on the same filesystem. Sparse dir must not exist.
```

### Tree Struct Diff

```
Usage: build/bin/tree_struct_diff [OPTION]... DIRECTORY DIRECTORY
  Compares the structure of two directory trees comparing
  the file names present or absent in each (sub-)directory.
  Options are...
    -C   to disable normalising the given paths.
    -s   to also check file sizes for differences.
    -t   to also check file types for differences.
  File types are 'directory', 'file', etc.
```

## The Smart Hash

The "smart hash" used throughout these tools is a way to speed up hashing when it can be assumed that two files with the same size are either identical *or* sufficiently different to make this difference apparent in a partial hash.

For file extensions corresponding to (usually) compressed file formats like `.mov`, `.bz2` and `.jpeg` the header [`hash_size.hpp`](https://github.com/ColinH/Filez/blob/main/src/hash_size.hpp) contains a mapping from file extension to *chunk size*.

The smart hash for a file that does *not* have a defined *chunk size* is the hash of the entire file, i.e. the same as the "total" hash.
This can be considered the default case.

The smart hash for a file that *does* have a defined *chunk size* in the aforementioned header also depends on the size of the file.

* If the file is smaller than 3 times the *chunk size* then the entire file is hashed.

* If the file is larger than 3 times the *chunk size* then only the *chunk size* first and last bytes of the file are hashed.

* If the file is larger than 1024 times the *chunk size* then an additional *chunk size* bytes near the centre of the file are also included in the smart hash.

Note that due to page alignment and/or rounding sizes up to the system page size, slightly more data than indicated might be included in the smart hash.

## Limitations

Currently soft links (symbolic links) are always ignored and never followed.

Not everything that should be configurable via command line arguments is configurable -- yet.

The smart hash is a heuristic that might not always be appropriate.

Copyright (c) 2022-2024 Dr. Colin Hirsch
