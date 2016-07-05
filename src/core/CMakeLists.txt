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

# The sources of SatCore
set( SAT_CORE_SRCS
	FormulaObject.cpp
	Variable.cpp
	Literal.cpp
	RawLiteral.cpp
	Clause.cpp
	Formula.cpp
	Valuation.cpp
	HistoryStep.cpp
	RemoveClauseStep.cpp
	RemoveLiteralFromClauseStep.cpp
	History.cpp
	ListenerDispatcher.cpp
	HistoryBasedDpllSolver.cpp
	ResolutionStackLevel.cpp
	ResolutionStack.cpp
	IterativeDpllSolver.cpp
	VariablePolarityLiteralSelector.cpp
	CnfLoader.cpp
	BasicSolutionChecker.cpp
	NoopSolverListener.cpp
)
set( SAT_CORE_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE )

# Build libSatCore
add_library( SatCore ${SAT_CORE_SRCS} )
target_compile_options( SatCore PUBLIC )
target_link_libraries(  SatCore ${LIBS} )
