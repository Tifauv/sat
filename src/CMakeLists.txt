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

# Declare loc4c as mandatory dependency
find_package( Log4C REQUIRED )
include_directories( ${LOG4C_INCLUDE_DIRS} )
set( CMAKE_INCLUDE_CURRENT_DIR on )
set( LIBS ${LIBS} ${LOG4C_LIBRARIES} )

# Require C++11 standard by default
set( CMAKE_CXX_STANDARD 11 )
set( CMAKE_CXX_STANDARD_REQUIRED on )

# Declare common CFLAGS
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic" )
set( CMAKE_CXX_FLAGS_DEBUG          "-O0 -ggdb" )
set( CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -g" )
set( CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g" )
set( CMAKE_CXX_FLAGS_RELEASE        "-O2" )

# Default to the Release build type
if( NOT CMAKE_BUILD_TYPE )
	set( CMAKE_BUILD_TYPE Release CACHE STRING
		"Choose the type of build between: None, Debug, Release, RelWithDebInfo, MinSizeRel."
		FORCE )
endif()

# Build libraries
add_subdirectory( core )
add_subdirectory( heuristics )
add_subdirectory( listeners )

# Include the libraries include directories
include_directories( ${SAT_CORE_INCLUDE} )
include_directories( ${SAT_HEURISTICS_INCLUDE} )
include_directories( ${SAT_LISTENERS_INCLUDE} )

# The sources of solveSat
set( SAT_SOLVER_SRCS
	solveSat.cpp
)

# The sources of checkSat
set( SAT_CHECKER_SRCS
	checkSat.cpp
)

# Build solveSat
add_executable( solveSat ${SAT_SOLVER_SRCS} )
target_compile_options( solveSat PRIVATE )
target_link_libraries(  solveSat LINK_PUBLIC SatCore SatHeuristics SatListeners ${LIBS} )

# Build checkSat
add_executable( checkSat ${SAT_CHECKER_SRCS} )
target_compile_options( checkSat PRIVATE )
target_link_libraries(  checkSat LINK_PUBLIC SatCore ${LIBS} )
