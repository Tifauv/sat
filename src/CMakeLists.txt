# Declare loc4c as mandatory dependency
find_package(Log4C REQUIRED)
include_directories(${LOG4C_INCLUDE_DIRS})
set(CMAKE_INCLUDE_CURRENT_DIR on)
set(LIBS ${LIBS} ${LOG4C_LIBRARIES})

# Declare common CFLAGS
set(CFLAGS -Wall -Wextra -pedantic -std=c++11 -O2)

# Build libraries
add_subdirectory (core)
add_subdirectory (heuristics)

# Include the libraries include directories
include_directories(${SAT_CORE_INCLUDE})
include_directories(${SAT_HEURISTICS_INCLUDE})

# The sources of solveSat
set (SAT_SOLVER_SRCS
	solveSat.cpp
)

# The sources of checkSat
set (SAT_CHECKER_SRCS
	checkSat.cpp
)

# Build solveSat
add_executable (solveSat ${SAT_SOLVER_SRCS})
target_compile_options(solveSat PRIVATE ${CFLAGS})
target_link_libraries (solveSat
		LINK_PUBLIC SatCore
		LINK_PUBLIC SatHeuristics
		${LIBS})

# Build checkSat
add_executable (checkSat ${SAT_CHECKER_SRCS})
target_compile_options(checkSat PRIVATE ${CFLAGS})
target_link_libraries (checkSat LINK_PUBLIC SatCore SatHeuristics ${LIBS})
