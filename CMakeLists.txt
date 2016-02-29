#  Copyright 2015 Olivier Serve
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111 USA

cmake_minimum_required( VERSION 3.1 )
project( Sat VERSION 2.2 LANGUAGES CXX )

# Load CMake modules
set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
		"${CMAKE_SOURCE_DIR}/cmake/Modules/"
)

# Default to the Release build type
if( NOT CMAKE_BUILD_TYPE )
	set( CMAKE_BUILD_TYPE Release CACHE STRING
		"Choose the type of build between: None, Debug, Release, RelWithDebInfo, MinSizeRel."
		FORCE )
endif()

# Build options
option( USE_CLANG  "Build application with clang"   off )
option( WITH_LOG4C "Enable the dependency on Log4c" off )


# Declare common CFLAGS
if( USE_CLANG )
	set( CMAKE_CXX_FLAGS                "-Wall" )
	set( CMAKE_CXX_FLAGS_DEBUG          "-O1 -g -fsanitize=address -fno-omit-frame-pointer" )
	set( CMAKE_CXX_FLAGS_MINSIZEREL     "-Os" )
	set( CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g" )
	set( CMAKE_CXX_FLAGS_RELEASE        "-O3" )
else( USE_CLANG )
	set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic" )
	set( CMAKE_CXX_FLAGS_DEBUG          "-O0 -ggdb" )
	set( CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -march=native" )
	set( CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -march=native -g" )
	set( CMAKE_CXX_FLAGS_RELEASE        "-O2 -march=native" )
endif( USE_CLANG )

# Declare log4c as a mandatory dependency if it is explicitely enabled
if( WITH_LOG4C )
	find_package( Log4C REQUIRED )
	include_directories( ${LOG4C_INCLUDE_DIRS} )
	set( CMAKE_INCLUDE_CURRENT_DIR on )
	set( LIBS ${LIBS} ${LOG4C_LIBRARIES} )
else( WITH_LOG4C )
	add_definitions(-DWITHOUT_LOG4C)
endif( WITH_LOG4C )


# Directories to build
add_subdirectory( src )
add_subdirectory( doc )
