include_directories(${SAT_CORE_INCLUDE})

# The sources of SatHeuristics
set (SAT_LISTENERS_SRCS
	BacktrackCounterListener.cpp
)
set (SAT_LISTENERS_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)

# Build libSatListeners
add_library (SatListeners ${SAT_LISTENERS_SRCS})
target_compile_options(SatListeners PUBLIC ${CFLAGS})
target_link_libraries (SatListeners LINK_PUBLIC SatCore ${LIBS})
