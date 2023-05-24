add_library(myanalysis
        dataflow/UseBeforeDef.cpp
        Analysis.cpp
        AnalysisFactory.cpp
        AnalysisResult.cpp
        AlwaysFailedAnalysis.cpp
        UseBeforeDefAnalysis.cpp
        ArithmeticIntensityAnalysis.cpp
        DeadCodeEliminationAnalysis.cpp
        MemoryLeakAnalysis.cpp
        dataflow/MemoryLeak.cpp)

target_link_libraries(myanalysis
        libanalyzer
        )

find_package(LLVM REQUIRED CONFIG)

target_include_directories(myanalysis
        PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/libanalyzer/include
        ${LLVM_INCLUDE_DIRS}
        )
