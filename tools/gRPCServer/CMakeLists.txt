add_executable(algServer server.cpp)

target_link_libraries(algServer
        algproto
        myanalysis
)

target_include_directories(algServer
        PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        )
