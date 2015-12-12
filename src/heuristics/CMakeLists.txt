include_directories(${SAT_CORE_INCLUDE})

# The sources of SatHeuristics
set (SAT_HEURISTICS_SRCS
	FirstVariableSelector.cpp
	MostUsedVariableSelector.cpp
	PositiveFirstPolaritySelector.cpp
)
set (SAT_HEURISTICS_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)

# Build libSatHeuristics
add_library (SatHeuristics ${SAT_HEURISTICS_SRCS})
target_compile_options(SatHeuristics PUBLIC ${CFLAGS})
target_link_libraries (SatHeuristics LINK_PUBLIC SatCore ${LIBS})
