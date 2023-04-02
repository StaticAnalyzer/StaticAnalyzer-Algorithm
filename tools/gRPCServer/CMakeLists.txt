add_executable(algServer server.cpp)

target_link_libraries(algServer
  algproto
  analysis
  framework
  ${CLANG_LIBS}
  ${LLVM_LIBS}
  ${OTHER_LIBS}
  stdc++fs
)
