add_executable(mytests
        main.cpp
        TestInitialization.cpp)

target_link_libraries(mytests
        myanalysis
        )

target_include_directories(mytests
        PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        )

add_test(NAME doctest COMMAND mytests)

set_tests_properties(doctest
        PROPERTIES
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        )

