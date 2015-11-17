# Declare loc4c as mandatory dependency
find_package(Log4C REQUIRED)
include_directories(${LOG4C_INCLUDE_DIRS})
set(LIBS ${LIBS} ${LOG4C_LIBRARIES})

# The sources of testSat
set (SRCS
	literal.cpp
	interpretation.cpp
	history.cpp
	sat.cpp
	alg_dp.cpp
	testSat.cpp
)

# Build testSat
add_executable (testSat ${SRCS})
target_compile_options(testSat PRIVATE -Wall -pedantic -std=c++11)
target_link_libraries (testSat ${LIBS})
