
DESCRIPTION (Revo Exercise)
============================

Implement Huffman compression algorithm. Two command line utilities:

1. 'encode' -- encodes any input file using Huffman algorithm:

> encode <input-file> <output-file>

2. 'decode' -- decodes input file to the original one:

> decode <input-file> <output-file>

Requirements:

* Pure C or C++ (up to C++14). No any extensions, only standard language
  features. No assembly code.
* Makefile as a build system. Don't use CMake! Executing 'make' should
  build 'encode' and 'decode' binaries.
* Only standard libraries included with the compiler, i.e. C standard library or
  C++ standard library (STL). No Boost! No Qt!
* Encoding and decoding should work as fast as possible.
 Binaries should not crash (e.g. Segmentation Fault) on any* inputs.

HOW-TO BUILD
============

## Linux ##
```
$ make
```

#### Tests ####

If google test framework installed:
```
$ make test
```

Integration tests
```
$ ./batch_test.sh
```

## Windows ##

In Visual Studio command prompt
```
$ nmake -f Makefile.nmake
```

#### Tests ####

If google test framework installed:

Define environment variables:

* `GTEST_ROOT=...`      - Google test root directory.
* `GMOCK_LIB_ROOT=...`  - Path to Google test/mock _debug_ libs `gtestd.lib` `gmockd.lib` `gtest_maind.lib`

```
$ nmake -f Makefile.nmake test
```

Integration tests

* `md5sum.exe` - must be in PATH

```
$ ./batch_test.cmd
```

TODO:
=====

* Optimizations
* Non-char symbol type
* CMake

REFS:
=====
https://ru.coursera.org/learn/algorithms-part2/lecture/6Hzrx/huffman-compression
https://en.wikipedia.org/wiki/Huffman_coding


ABOUT
=====
:cat: Written from scratch by @svak

(c) 2018 @svak All rights reserved
