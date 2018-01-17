#!/bin/sh

srcdir=$(readlink --canonicalize $(dirname $0) )
builddir=$(readlink --canonicalize "${srcdir}/build-clang" )

echo ">> Sources in ${srcdir}"
echo ">> Build   in ${builddir}"

# Create the build directory if needed
if [ ! -d ${builddir} ]
then
	mkdir ${builddir}
fi

# Move into build directory
cd ${builddir}

# Set the CXX environment variable
export CXX=$(which clang++)

# Generate the Makefiles
cmake $@ -DUSE_CLANG=on -D_CMAKE_TOOLCHAIN_PREFIX=llvm- -DCMAKE_BUILD_TYPE=Debug ${srcdir}

# Build
make -j1
