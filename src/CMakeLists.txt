# Declare loc4c as mandatory dependency
find_package(Log4C REQUIRED)
include_directories(${LOG4C_INCLUDE_DIRS})
set(LIBS ${LIBS} ${LOG4C_LIBRARIES})

# Declare common CFLAGS
set(CFLAGS -Wall -Wextra -pedantic -std=c++11 -O2)

# The sources of libSat
set (LIBSAT_SRCS
	formulaobject.cpp
	variable.cpp
	literal.cpp
	rawliteral.cpp
	clause.cpp
	formula.cpp
	interpretation.cpp
	history.cpp
	dpllsolver.cpp
	cnfloader.cpp
)

# The sources of solveSat
set (SOLVESAT_SRCS
	solveSat.cpp
)

# The sources of checkSat
set (CHECKSAT_SRCS
	checkSat.cpp
)

# Build libSat
add_library (sat ${LIBSAT_SRCS})
target_compile_options(sat PUBLIC ${CFLAGS})
target_link_libraries (sat ${LIBS})

# Build solveSat
add_executable (solveSat ${SOLVESAT_SRCS})
target_compile_options(solveSat PRIVATE ${CFLAGS})
target_link_libraries (solveSat LINK_PUBLIC sat ${LIBS})

# Build checkSat
add_executable (checkSat ${CHECKSAT_SRCS})
target_compile_options(checkSat PRIVATE ${CFLAGS})
target_link_libraries (checkSat LINK_PUBLIC sat ${LIBS})

