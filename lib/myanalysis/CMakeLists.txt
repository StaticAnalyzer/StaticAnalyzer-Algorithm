add_library(analysis
        Analysis.cpp
        AnalysisFactory.cpp
        AnalysisResult.cpp
        UninitializedVariableAnalysis.cpp
        )

target_link_libraries(analysis
    libanalyzer
)
