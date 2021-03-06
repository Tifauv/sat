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

include_directories( ${SAT_CORE_INCLUDE} )

# The sources of SatHeuristics
set( SAT_LISTENERS_SRCS
	StatisticsListener.cpp
	ChronoListener.cpp
)
set( SAT_LISTENERS_HEADERS
	StatisticsListener.h
	ChronoListener.h
)
# Add the LoggingListener if compiled with -DWITH_LOGGING
if( WITH_LOGGING )
	set( SAT_LISTENERS_SRCS
		${SAT_LISTENERS_SRCS}
		LoggingListener.cpp
	)
	set( SAT_LISTENERS_HEADERS
			${SAT_LISTENERS_HEADERS}
		LoggingListener.h
	)
endif( WITH_LOGGING )


set( SAT_LISTENERS_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE )

# Build libSatListeners
add_library( SatListeners ${SAT_LISTENERS_SRCS} )
target_compile_options( SatListeners PUBLIC )
target_link_libraries(  SatListeners LINK_PUBLIC SatCore ${LIBS} )
