include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_INCLUDE_DIR})

set (SYSTEM_LIBS log4c)

set (SRCS
	literal.cpp
	interpretation.cpp
	history.cpp
	sat.cpp
	alg_dp.cpp
	testSat.cpp
)

add_executable (testSat ${SRCS})
target_compile_options(testSat PRIVATE -Wall -pedantic -std=c++11)
target_link_libraries (testSat ${SYSTEM_LIBS})
