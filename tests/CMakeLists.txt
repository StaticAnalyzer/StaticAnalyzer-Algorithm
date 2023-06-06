add_executable(mytests
        main.cpp
        TestInitialization.cpp
        TestUseBeforeDef.cpp
        TestArithmeticIntensityReduce.cpp
        TestDeadCodeElimination.cpp
        TestMemoryLeak.cpp
        TestNullPointer.cpp)

target_link_libraries(mytests
        myanalysis
        )

target_include_directories(mytests
        PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        )

add_test(NAME mydoctest COMMAND mytests)

set_tests_properties(mydoctest
        PROPERTIES
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        )

