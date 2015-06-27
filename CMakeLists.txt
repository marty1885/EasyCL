cmake_minimum_required (VERSION 2.6)

project(EasyCl)
project(example1)
project(upload)
project(reallySmallRSACrack)
project(hugeData)

option(DEBUG_MODE "Enable debug flags an others" OFF)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
find_package(OpenCL REQUIRED)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(include)
include_directories(${OPENCL_INCLUDE_DIRS})

#Make sure we have OCL C++ Wrapper.
if(${OPENCL_HAS_CPP_BINDINGS} STREQUAL "TRUE")
	message( STATUS "OpenCL C++ Wrapper founded! Able to build")
else()
	message( FATAL_ERROR "No OpenCL C++ Wrapper found. Will now exit.")
endif()

#setup some debug flags
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} " -std=c++11")
if(DEBUG_MODE)
	set(${CMAKE_CXX_FLAGS} " -O3 -ffast-math")
else()
	set(${CMAKE_CXX_FLAGS} " -O -g")
endif()

add_library(EasyCl SHARED src/EasyCl.cpp)
target_link_libraries(EasyCl ${OPENCL_LIBRARIES})

add_executable(example1 example/example1.cpp)
target_link_libraries(example1 ${OPENCL_LIBRARIES})
target_link_libraries(example1 EasyCl)

add_executable(upload example/upload.cpp)
target_link_libraries(upload ${OPENCL_LIBRARIES})
target_link_libraries(upload EasyCl)

add_executable(reallySmallRSACrack example/reallySmallRSACrack.cpp)
target_link_libraries(reallySmallRSACrack ${OPENCL_LIBRARIES})
target_link_libraries(reallySmallRSACrack EasyCl)

add_executable(hugeData example/hugeData.cpp)
target_link_libraries(hugeData ${OPENCL_LIBRARIES})
target_link_libraries(hugeData EasyCl)

install(TARGETS EasyCl EasyCl DESTINATION /usr/lib)
install(FILES include/EasyCl.h DESTINATION /usr/include)

message(${OPENCL_LIBRARIES})
