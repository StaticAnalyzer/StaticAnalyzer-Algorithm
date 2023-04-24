cmake_minimum_required(VERSION 3.14)

project(se-experiment)

set(CMAKE_CXX_STANDARD 17)

include_directories(include libanalyzer/include)

add_subdirectory(libanalyzer)

add_subdirectory(lib)

add_subdirectory(tools)
