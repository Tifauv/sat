# The sources of SatCore
set (SATCORE_SRCS
	FormulaObject.cpp
	Variable.cpp
	Literal.cpp
	RawLiteral.cpp
	Clause.cpp
	Formula.cpp
	Interpretation.cpp
	History.cpp
	DpllSolver.cpp
	CnfLoader.cpp
)

# Build libSatCore
add_library (SatCore ${SATCORE_SRCS})
target_compile_options(SatCore PUBLIC ${CFLAGS})
target_link_libraries (SatCore ${LIBS})
