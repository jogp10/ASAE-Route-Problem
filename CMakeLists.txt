cmake_minimum_required(VERSION 3.17)
project(ASAEinspectionRoute)

set(CMAKE_CXX_STANDARD 14)

find_package(PythonInterp)
find_package(PythonLibs 2.7)

include_directories(${PYTHON_INCLUDE_DIR})

add_executable(
        ASAEinspectionRoute
        main.cpp

        src/graph/disjointSets.h src/graph/minHeap.h
        src/graph/graph.cpp src/graph/graph.h

        src/ASAE.cpp src/ASAE.h
)

target_include_directories(ASAEinspectionRoute PRIVATE ${PYTHON_INCLUDE_DIR})
target_link_libraries(ASAEinspectionRoute ${PYTHON_LIBRARIES})