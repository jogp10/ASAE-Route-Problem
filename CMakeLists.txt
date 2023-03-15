cmake_minimum_required(VERSION 3.14)
project(ASAEinspectionRoute)

add_subdirectory(matplotplusplus)
add_executable(
        ASAEinspectionRoute
        main.cpp

        src/graph/disjointSets.h src/graph/minHeap.h
        src/graph/graph.cpp src/graph/graph.h

        src/ASAE.cpp src/ASAE.h
)

target_link_libraries(ASAEinspectionRoute PUBLIC matplot)

set_target_properties(ASAEinspectionRoute PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
        )
