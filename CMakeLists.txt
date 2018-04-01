cmake_minimum_required(VERSION 2.4 FATAL_ERROR)

project(T49_TO_DT)

set(CMAKE_BUILD_TYPE debug)

#set(NA49 /afs/cern.ch/work/o/ogolosov/public/NA49/ROOT_NA49)
#set(DATATREE /afs/cern.ch/work/o/ogolosov/public/DataTree)

set(NA49 /home/ogolosov/Desktop/Analysis/ROOT_NA49)
set(DATATREE /home/ogolosov/Desktop/Analysis/DataTree)

list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})

find_package(ROOT REQUIRED)

include(${ROOT_USE_FILE})

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${ROOT_INCLUDE_DIRS}
    ${NA49}/T49DST/inc
    ${DATATREE}/src
    )
add_definitions(${ROOT_CXX_FLAGS})

add_executable(T49_to_DT productionMap.h particleCodes.h main.cpp)
target_link_libraries(T49_to_DT
    ${ROOT_LIBRARIES}
    ${DATATREE}/build/libDataTree.so
    ${NA49}/build/T49DST/libT49DST.so
    )
