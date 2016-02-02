# The sources of SatCore
set (SAT_CORE_SRCS
	FormulaObject.cpp
	Variable.cpp
	Literal.cpp
	RawLiteral.cpp
	Clause.cpp
	Formula.cpp
	Interpretation.cpp
	History.cpp
	HistoryBasedDpllSolver.cpp
	VariablePolarityLiteralSelector.cpp
	CnfLoader.cpp
	BasicSolutionChecker.cpp
	NoopSolverListener.cpp
)
set (SAT_CORE_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)

# Build libSatCore
add_library (SatCore ${SAT_CORE_SRCS})
target_compile_options(SatCore PUBLIC ${CFLAGS})
target_link_libraries (SatCore ${LIBS})
