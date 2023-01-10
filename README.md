# Filez

A couple of file tools for the Unix shell.

 * [Duplicates](#duplicates) -- Find duplicate files.
 * [Variations](#variations) -- Find (meta-)data variations.
 * [Deduplicate](#deduplicate) -- Copy tree with hard links instead of duplicate files.
 * [Incremental](#incremental) -- Incremental backup with multiple search strategies.
 * [Tree Struct Diff](#tree-struct-diff) -- Compares structure of two file-and-directory trees.

Implemented in C++20 using the C and C++ standard libraries and Posix standard functions.

## Building

Requires a Unix system with GNU make and a C++20 capable Clang or GCC.

Enter `make` in the main directory, the binaries can then be found under `build/bin`.

Tested with Clang 14 from Xcode 14 under macOS 13 on ARM64, and with GCC 12 under Linux on x86-64.

### Linux

Please consult your distribution's manual on how to install GCC.
If the version included with your distribution does not have enough C++20 support you might need to search the web for other solutions.

### macOS

If the `clang++` included with your Xcode does not support C++20 you will need something more recent, for example using [Homebrew](https://brew.sh) to install the latest LLVM.

### Others

If you are trying to build this on any other Unix or Unix-like system like one of the BSDs you probably know how to install or build yourself a suitable compiler...

## The Tools

None of these tools ever delete or overwrite any existing filesystem object.

### Duplicates

Recursively scans one or more directories looking for duplicate files.

The following definitions of "duplicate" are available:

 * Same file name.
 * Same file name and file size.
 * Same file name and file size and [smart hash](#the-smart-hash).
 * Same file name and file size and total hash.
 * Same device and inode.
 * Same [smart hash](#the-smart-hash) and file size.
 * Same total hash and file size.

All duplicates are printed with their paths.

All available options are shown when invoked without command line arguments.

### Variations

Finds (meta) data variations where files that share certain attributes differ in others.

The following variations can be chosen from:

 * Files with the same file name but different size.
 * Files with the same file name but different [smart hash](#the-smart-hash).
 * Files with the same file name but different total hash.
 * Files with the same device and inode but different file name.
 * Files with the same [smart hash](#the-smart-hash) but different file name.
 * Files with the same total hash but different file name.
 * Files with the same [smart hash](#the-smart-hash) but different device and/or inode.
 * Files with the same total hash but different device and/or inode.

All variations are printed with their paths.

All available options are shown when invoked without command line arguments.

### Deduplicate

Creates a copy of a directory tree where files are hard links to their originals.

Identical files, as determined by file size and smart or total hash, will hard link to the same inode.

All available options are shown when invoked without command line arguments.

### Incremental

Creates a new backup from a source directory and zero or more old backups.

Files that exist in the old backups are hard linked into the new backup instead of copying from the source.

Files for hard linking from an old backup to the new backup are found by:

 * Matching file size and [smart hash](#the-smart-hash).
 * Matching file size and total hash.
 * Matching file size and file name.
 * Matching file size and file name and [smart hash](#the-smart-hash).
 * Matching file size and file name and total hash.
 * Matching file size and relative path within source_dir and an old_backup.

All available options are shown when invoked without command line arguments.

At least one of the search strategies for hard linking has to be enabled.

Optionally considers freshly copied files for hard linking to perform on-the-fly deduplication in the backup.

The backup directories must all be on the same filesystem, and the source directory must be on a different one.

Unlike many other backup applications this one never consults any of the file timestamps to determine anything.

### Tree Struct Diff

Recursively compares two directory trees and prints all structural differences.

A structural difference is when a filesystem object of a certain name exists on one side but not the other.

Recurses into directories when a directory of the same name exists on both sides.

## The Smart Hash

The "smart hash" used throughout these tools is a way to speed up hashing when it can be assumed that two files with the same size are either identical or "sufficiently different".

For file extensions corresponding to (usually) compressed file formats like `.mov`, `.bz2` and `.jpeg` the file [`hash_size.hpp`](https://github.com/ColinH/Filez/blob/main/src/hash_size.hpp) contains a mapping from file extension to chunk size.
The smart hash for a file that does not have a "configured" chunk size is the hash of the entire file.

For files with a configured chunk size the smart hash also depends on the file size.
If the file is smaller than 3 times the chunk size then the entire file is hashed.
Otherwise the beginning and end of the file are hashed, using (at least) chunk size bytes at either end (can be more due to page alignment).
If the file is larger than 1024 times the chunk size then chunk size bytes at the centre of the file are also included in the hash.

## Limitations

Currently soft links are always ignored and never followed.

Not everything that should be configurable via command line arguments is actually configurable -- yet.

Copyright (c) 2022-2023 Dr. Colin Hirsch
