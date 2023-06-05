cmake_minimum_required(VERSION 2.9 FATAL_ERROR)

project(t49toTTree)

set(CMAKE_BUILD_TYPE debug)

list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})

find_package(ROOT REQUIRED COMPONENTS Core Tree EG)

include(${ROOT_USE_FILE})

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${ROOT_INCLUDE_DIRS}
    $ENV{T49DST}/inc
    )
add_definitions(${ROOT_CXX_FLAGS})

add_executable(t49toTTree productionMap.h t49toTTree.h t49toTTree.cpp)
target_link_libraries(t49toTTree
    ${ROOT_LIBRARIES}
    $ENV{T49}/build/T49DST/libT49DST.so
    )
