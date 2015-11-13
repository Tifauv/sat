#!/bin/sh

srcdir=$(readlink --canonicalize $(dirname $0) )
builddir=$(readlink --canonicalize "${srcdir}/build" )

echo ">> Sources in ${srcdir}"
echo ">> Build   in ${builddir}"

# Delete the build directory if it exists
if [ -d ${builddir} ]
then
	rm -rf ${builddir}
	echo "   Build directory deleted."
else
	echo "   Build directory does not exist, nothing to do."
fi
