cmake_minimum_required(VERSION 3.14)
project(ASAEinspectionRoute)

#add_subdirectory(matplotplusplus)

include(FetchContent)
FetchContent_Declare(matplotplusplus
        GIT_REPOSITORY https://github.com/alandefreitas/matplotplusplus
        GIT_TAG origin/master) # or whatever tag you want
FetchContent_GetProperties(matplotplusplus)
if(NOT matplotplusplus_POPULATED)
        FetchContent_Populate(matplotplusplus)
        add_subdirectory(${matplotplusplus_SOURCE_DIR} ${matplotplusplus_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

add_executable(
        ASAEinspectionRoute
        main.cpp

        src/graph/graph.cpp src/graph/graph.h

        src/ASAE.cpp src/ASAE.h
        src/matplot/geoplot_draw.cpp src/matplot/geoplot_draw.h
        src/graph/minHeap.h
        src/graph/Time.cpp src/graph/Time.h)

target_link_libraries(ASAEinspectionRoute PUBLIC matplot)
