cmake_minimum_required(VERSION 3.25)
project(CAD_final_project)

set(CMAKE_CXX_STANDARD 14)

include_directories(src)
add_executable(CAD_final_project
        src/createSteiner_tb.cpp
        src/datastructure.h
        src/findPath.h
        src/rebound.h
        src/Steiner.cpp
        src/Steiner.h
        src/util.h)
