# Created by the script cgal_create_CMakeLists
# This is the CMake script for compiling a set of CGAL applications.

cmake_minimum_required(VERSION 3.1...3.15)

project(classificate_train)

# CGAL and its components
find_package( CGAL QUIET COMPONENTS  )
if ( NOT CGAL_FOUND )
  message(STATUS "This project requires the CGAL library, and will not be compiled.")
endif()

# Boost and its components
find_package( Boost QUIET )
if ( NOT Boost_FOUND )
  message(STATUS "This project requires the Boost library, and will not be compiled.")
  return()  
endif()

# include for local directory
# include for local package

# libpointmatcher
find_package(libpointmatcher REQUIRED)
include_directories(${CMAKE_CURRENT_BINARY_DIR} ${libpointmatcher_INCLUDE_DIRS})
message(STATUS "Using libpointmatcher version ${libpointmatcher_VERSION}")
# add_executable(${PROJECT_NAME} classificate.cpp)
add_executable(test test.cpp)
add_executable(train train.cpp)

find_package(Eigen3 REQUIRED NO_MODULE)
INCLUDE_DIRECTORIES(${EIGEN3_INCLUDE_DIR})

# tbb
find_package(TBB REQUIRED)

find_package(GMP REQUIRED)

target_link_libraries(test
                      ${libpointmatcher_LIBRARIES}
                      ${gmp_LIBRARIES}
                      TBB::tbb)

target_link_libraries(train
                      ${libpointmatcher_LIBRARIES}
                      ${gmp_LIBRARIES}
                      TBB::tbb)
