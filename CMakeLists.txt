cmake_minimum_required(VERSION 3.14)

project(se-experiment)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(libanalyzer)

add_subdirectory(lib)

add_subdirectory(tools)
