aux_source_directory(. analysis_srcs)

add_library(analysis ${analysis_srcs})

target_link_libraries(analysis
    libanalyzer
)
