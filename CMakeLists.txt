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
project( Sat VERSION 2.1 LANGUAGES CXX )

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

add_subdirectory( src )
add_subdirectory( doc )
