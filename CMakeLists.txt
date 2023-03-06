cmake_minimum_required(VERSION 3.20)
project(ASAEinspectionRoute)

set(CMAKE_CXX_STANDARD 14)

add_executable(
        ASAEinspectionRoute
        main.cpp

        src/graph/disjointSets.h src/graph/minHeap.h
        src/graph/graph.cpp src/graph/graph.h

        src/Menu.cpp src/Menu.h
        src/ASAE.cpp src/ASAE.h
)