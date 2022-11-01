#!/bin/sh

srcdir=$(readlink --canonicalize $(dirname $0) )
builddir=$(readlink --canonicalize "${srcdir}/build" )

echo ">> Sources in ${srcdir}"
echo ">> Build   in ${builddir}"

# Create the build directory if needed
if [ ! -d ${builddir} ]
then
	mkdir ${builddir}
fi

# Move into build directory
cd ${builddir}

# Generate the Makefiles
cmake -DCMAKE_BUILD_TYPE=Debug -DWITH_LOGGING=on ${srcdir}

# Build
make -j1
