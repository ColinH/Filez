# Filez

A couple of file tools for the Unix shell.

 * [Duplicates](#duplicates) finds duplicate files.
 * [Variations](#variations) does something similar.
 * [Incremental](#incremental) for efficient incremental backups.
 * [Tree Struct Diff](#tree-struct-diff) compares two directory trees.

Implemented in C++20 using the C and C++ standard libraries and Posix standard functions...

...and some macOS specific functions for the hash, so currently only compiles under macOS.

## Building

Enter `make` in the Terminal on a Mac with the Xcode command line tools installed.

The binaries can then be found under `build/bin`.

## The Tools

None of these tools ever delete or overwrite any existing filesystem object.

### Duplicates

Recursively scans one or more directories looking for duplicate files.

The following definitions of "duplicate" are available:

 * Same file name.
 * Same file name and file size.
 * Same file name and file size and smart hash.
 * Same file name and file size and total hash.
 * Same device and inode.
 * Same smart hash and file size.
 * Same total hash and file size.

All duplicates are printed with their paths.

All available options are shown when invoked without command line arguments.

### Variations

Finds (meta) data variations where files that share certain attributes differ in others.

The following variations can be chosen from:

 * Files with the same file name but different size.
 * Files with the same file name but different smart hash.
 * Files with the same file name but different total hash.
 * Files with the same device and inode but different file name.
 * Files with the same smart hash but different file name.
 * Files with the same total hash but different file name.
 * Files with the same smart hash but different device and/or inode.
 * Files with the same total hash but different device and/or inode.

All variations are printed with their paths.

All available options are shown when invoked without command line arguments.

### Incremental

Creates a new backup from a source directory and zero or more old backups.

Files that exist in the old backups are hard linked into the new backup instead of copying from the source.

Files for hard linking from an old to the new backup are found by file size and smart hash (not path or name).

The backup directories must all be on the same filesystem, and the source directory must be on a different one.

Unlike many other backup applications this one never consults any of the file timestamps to determine anything.

### Tree Struct Diff

Recursively compares two directory trees and prints all structural differences.

A structural difference is when a filesystem object of a certain name exists on one side but not the other.

Recurses into directories when a directory of the same name exists on both sides.

## The Smart Hash

The "smart hash" used throughout these tools is a way to speed up hashing when it can be safely assumed that files are either the same or "very different".

For file extensions corresponding to (usually) compressed file formats like `.mov`, `.bz2` and `.jpeg` the file `hash_size.hpp` contains a mapping from file extension to chunk size.
This chunk size is currently 4KB for audio and image files, 16KB for video files, and 64KB for compressed archives.
The smart hash for a file that does not have a "configured" chunk size is simply the hash of the entire file.

For files with a configured chunk size the smart hash also depends on the file size.
If the file is smaller than 3 times the chunk size then the entire file is hashed.
Otherwise the beginning and end of the file are hashed, using (at least) chunk size bytes at either end (can be more due to page alignment).
If the file is larger than 1024 times the chunk size then chunk size bytes at the centre of the file are also included in the hash.

## Limitations

Currently only compiles on macOS, with Linux support possibly coming soon.

Does not give the user the choice between 'follow' and 'ignore' for soft links.

Copyright (c) 2022-2023 Dr. Colin Hirsch
