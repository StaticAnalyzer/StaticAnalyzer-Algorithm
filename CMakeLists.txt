cmake_minimum_required(VERSION 3.14)

project(seexperiment)

set(CMAKE_CXX_STANDARD 17)

include_directories(include cpp-static-analyzer/include)

add_subdirectory(libanalyzer)

add_subdirectory(lib)

add_subdirectory(tools)
