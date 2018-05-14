Sat
===

# Compiling

## Requirements
- gcc or clang supporting C++11
- CMake 3.1
- log4c (optional)

## With gcc
Run the `build.sh` script to build the project using gcc, or run the following commands:
~~~
$ mkdir build
$ cd build
$ cmake ..
$ make
~~~

## With clang
Run the `build-clang.sh` script to build the project using clang or run the following commands:
~~~
$ mkdir build
$ cd build
$ export CXX=$(which clang++)
$ cmake $@ -DUSE_CLANG=on -D_CMAKE_TOOLCHAIN_PREFIX=llvm- ..
$ make
~~~

## Result
The `solver` executable is `build/src/solveSat`.

The `checker` executable is `build/src/checkSat`.
