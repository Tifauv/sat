#  Copyright 2022 Olivier Serve
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

# Include the libraries include directories
include_directories( ${SAT_CORE_INCLUDE} )
include_directories( ${SAT_HISTORY_INCLUDE} )
include_directories( ${SAT_ITERATIVE_SOLVER_INCLUDE} )
include_directories( ${SAT_HEURISTICS_INCLUDE} )
include_directories( ${SAT_LISTENERS_INCLUDE} )

# The sources of solveSudoku
set( SUDOKU_SRCS
	SudokuLoader.cpp
	solveSudoku.cpp
)
set( SUDOKU_HEADERS
	SudokuLoader.h
)

# Build solveSudoku
add_executable( solveSudoku ${SUDOKU_SRCS} )
target_compile_options( solveSudoku PRIVATE )
target_link_libraries(  solveSudoku LINK_PUBLIC SatCore SatIterativeSolver SatHeuristics SatListeners ${LIBS} )
