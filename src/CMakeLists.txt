# Declare loc4c as mandatory dependency
find_package(Log4C REQUIRED)
include_directories(${LOG4C_INCLUDE_DIRS})
set(LIBS ${LIBS} ${LOG4C_LIBRARIES})

# The sources of libSat
set (LIBSAT_SRCS
	literal.cpp
	interpretation.cpp
	history.cpp
	sat.cpp
	alg_dp.cpp
)

# The sources of solveSat
set (SOLVESAT_SRCS
	${LIBSAT_SRCS}
	solveSat.cpp
)

# The sources of checkSat
set (CHECKSAT_SRCS
	${LIBSAT_SRCS}
	checkSat.cpp
)

# Build solveSat
add_executable (solveSat ${SOLVESAT_SRCS})
target_compile_options(solveSat PRIVATE -Wall -pedantic -std=c++11)
target_link_libraries (solveSat ${LIBS})

# Build checkSat
add_executable (checkSat ${CHECKSAT_SRCS})
target_compile_options(checkSat PRIVATE -Wall -pedantic -std=c++11)
target_link_libraries (checkSat ${LIBS})

