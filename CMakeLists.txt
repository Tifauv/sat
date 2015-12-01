cmake_minimum_required (VERSION 3.0)
project (Sat)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
		"${CMAKE_SOURCE_DIR}/cmake/Modules/"
)

add_subdirectory (src)
